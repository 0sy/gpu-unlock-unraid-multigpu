// SPDX-License-Identifier: GPL-2.0-only
/* Firmware format validation and immutable assets for one coordination round. */
#include <linux/firmware.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/slab.h>
#include <crypto/sha2.h>

#include "diagnostic.h"
#include "firmware_610.h"
#include "recipes.h"

#define BOOTER_MAGIC 0x314c5442u
#define SEC2_BOOTER_FILE "gpu-unlock/610.43.03/booter_load_ga100.bin"
#define GSP_FILE "gpu-unlock/610.43.03/gsp_ga10x.bin"
#define GSP_BOOT_FILE "gpu-unlock/610.43.03/gsp_boot_ga102.bin"
#define GSP_BOOT_DESC_FILE "gpu-unlock/610.43.03/gsp_boot_desc_ga102.bin"

#define GSP_GA100_FILE "gpu-unlock/610.43.03/gsp_tu10x.bin"
#define BOOT_GA100_FILE "gpu-unlock/610.43.03/gsp_boot_ga100.bin"
#define BOOT_DESC_GA100_FILE "gpu-unlock/610.43.03/gsp_boot_desc_ga100.bin"
#define FALCON_LOADER_FILE "gpu-unlock/610.43.03/falcon_loader.bin"
static const struct gpu_unlock_610_fw_desc ga100_load = {
	.gsp = GSP_GA100_FILE, .boot = BOOT_GA100_FILE, .desc = BOOT_DESC_GA100_FILE,
	.signature_section = ".fwsignature_ga100", .boot_size = 4096, .direct = true,
};
static const struct gpu_unlock_610_fw_desc ga10x_load = {
	.gsp = GSP_FILE, .boot = GSP_BOOT_FILE, .desc = GSP_BOOT_DESC_FILE,
	.signature_section = ".fwsignature_ga10x", .boot_size = 24576, .direct = false,
};

const struct gpu_unlock_610_fw_desc *
gpu_unlock_610_select_firmware(const struct gu_card *recipe)
{
	if (!recipe)
		return NULL;
	return recipe->direct_boot ? &ga100_load : &ga10x_load;
}

/* CONFIG_FW_CACHE attaches name records to devres even though this is not
 * devm_request_firmware(). release_firmware() does not remove them, and
 * really_probe() refuses a device with any existing devres. Scope each
 * synchronous request so only its cache records are released; the returned
 * firmware remains owned by the caller.
 */
static int request_scoped(struct device *dev, const struct firmware **fw, const char *name)
{
	void *group;
	int ret, released;

	group = devres_open_group(dev, NULL, GFP_KERNEL);
	if (!group) {
		dev_err(dev, "asset request failed: %s error=%d\n", name, -ENOMEM);
		return -ENOMEM;
	}
	dev_dbg(dev, "asset request: %s\n", name);
	ret = request_firmware(fw, name, dev);
	released = devres_release_group(dev, group);
	if (ret)
		dev_err(dev, "asset request failed: %s error=%d\n", name, ret);
	dev_dbg(dev, "asset result: %s error=%d bytes=%zu\n",
		name, ret, ret ? (size_t)0 : (*fw)->size);
	if (!ret) {
		u8 digest[SHA256_DIGEST_SIZE];
		sha256((*fw)->data, (*fw)->size, digest);
		dev_dbg(dev, "asset identity: %s sha256=%*phN\n",
			 name, SHA256_DIGEST_SIZE, digest);
	}
	dev_dbg(dev, "asset cache cleanup: %s devres_released=%d\n", name, released);
	return ret;
}

static int validate_booter(const struct firmware *fw, struct gpu_unlock_booter_hdr *out)
{
	const u8 *data;
	u32 magic, version, image_size, sig_bytes;
	struct gpu_unlock_booter_hdr hdr;

	data = fw->data;
	if (fw->size < 72)
		return GU_DIAG(-EINVAL);
	memcpy(&magic, data, sizeof(magic));
	memcpy(&version, data + 4, sizeof(version));
	if (magic != BOOTER_MAGIC || version != 1)
		return GU_DIAG(-EINVAL);
	memcpy(&hdr, data + 8, sizeof(hdr));
	memcpy(&image_size, data + 8 + sizeof(hdr), sizeof(image_size));
	memcpy(&sig_bytes, data + 12 + sizeof(hdr), sizeof(sig_bytes));
	if (!hdr.ucode_id || hdr.num_apps != 1 || sig_bytes != hdr.sig_bytes ||
	    image_size != GU_BOOTER_IMAGE_SIZE ||
	    fw->size != 72u + image_size + sig_bytes)
		return GU_DIAG(-EINVAL);
	/* Pinned signed GA100 Booter geometry, used by both launch paths. */
	if (hdr.ucode_id != 3 || hdr.engine_id != 1 || hdr.num_sigs != 1 ||
	    sig_bytes != GU_BOOTER_SIG_TOTAL || hdr.os_code_off != 0 ||
	    hdr.os_code_size != 256 || hdr.os_data_off != 34560 ||
	    hdr.os_data_size != 25600 || hdr.app_code_off != 256 ||
	    hdr.app_code_size != 34304 || hdr.app_data_off != 256 ||
	    hdr.app_data_size != 0 || hdr.patch_loc != 35072)
		return GU_DIAG(-EINVAL);
	*out = hdr;
	return 0;
}

static void release_family(struct gpu_unlock_610_firmware *assets)
{
	release_firmware(assets->boot);
	release_firmware(assets->gsp);
	memset(assets, 0, sizeof(*assets));
}

static int prepare_family(struct pci_dev *pdev, struct gpu_unlock_610_firmware *assets,
			  const struct gpu_unlock_610_fw_desc *load)
{
	const struct firmware *desc = NULL;
	int ret;

	ret = request_scoped(&pdev->dev, &assets->gsp,
			     load->gsp);
	if (ret)
		return ret;
	ret = gpu_unlock_elf_section(assets->gsp->data, assets->gsp->size,
				    ".fwimage", &assets->image_span);
	if (ret)
		return ret;
	ret = gpu_unlock_elf_section(assets->gsp->data, assets->gsp->size,
				    load->signature_section, &assets->sig_span);
	if (ret)
		return ret;
	if (!assets->image_span.size || assets->image_span.size > 128ull << 20 ||
	    assets->sig_span.size != 4096)
		return -EINVAL;
	ret = gpu_unlock_radix_plan(assets->image_span.size, &assets->radix);
	if (ret)
		return ret;
	ret = request_scoped(&pdev->dev, &assets->boot, load->boot);
	if (ret)
		return ret;
	if (assets->boot->size != load->boot_size)
		return -EINVAL;
	ret = request_scoped(&pdev->dev, &desc, load->desc);
	if (ret)
		return ret;
	ret = gpu_unlock_boot_desc_parse(desc->data, desc->size,
					assets->boot->size, &assets->boot_desc);
	dev_dbg(&pdev->dev, "asset geometry: image=%llu signature=%llu boot=%u code=%#x data=%#x manifest=%#x parse=%d\n",
		(unsigned long long)assets->image_span.size, (unsigned long long)assets->sig_span.size,
		assets->boot_desc.image_size, assets->boot_desc.code_offset,
		assets->boot_desc.data_offset, assets->boot_desc.manifest_offset, ret);
	release_firmware(desc);
	return ret;
}

int gpu_unlock_610_load_shared(const struct gu_device *device, struct gpu_unlock_610_shared *shared)
{
	struct pci_dev *pdev = device->pdev;
	const struct gpu_unlock_610_fw_desc *load = device->firmware;
	struct gpu_unlock_610_firmware *assets;
	const struct firmware *booter = NULL, *loader = NULL;
	int ret;

	if (!device->recipe || !load)
		return -ENODEV;
	assets = load->direct ? &shared->ga100 : &shared->ga10x;
	if (assets->prepared)
		return assets->error;
	ret = prepare_family(pdev, assets, load);
	if (ret)
		goto out;
	if (!shared->booter) {
		ret = request_scoped(&pdev->dev, &booter, SEC2_BOOTER_FILE);
		if (ret)
			goto out;
		ret = validate_booter(booter, &shared->booter_hdr);
		if (ret) {
			dev_err(&pdev->dev, "invalid SEC2 Booter bundle: error=%d\n", ret);
			release_firmware(booter);
			goto out;
		}
		shared->booter = booter;
	}
	if (load->direct && !shared->loader) {
		ret = request_scoped(&pdev->dev, &loader, FALCON_LOADER_FILE);
		if (ret)
			goto out;
		if (loader->size != 768) {
			dev_err(&pdev->dev, "invalid generic Falcon loader: bytes=%zu\n", loader->size);
			release_firmware(loader);
			ret = -EINVAL;
			goto out;
		}
		shared->loader = loader;
	}
out:
	if (ret)
		release_family(assets);
	assets->error = ret;
	assets->prepared = true;
	return ret;
}

void gpu_unlock_610_release_shared(struct gpu_unlock_610_shared *shared)
{
	release_family(&shared->ga100);
	release_family(&shared->ga10x);
	release_firmware(shared->loader);
	release_firmware(shared->booter);
	memset(shared, 0, sizeof(*shared));
}

MODULE_FIRMWARE(SEC2_BOOTER_FILE);
MODULE_FIRMWARE(FALCON_LOADER_FILE);
MODULE_FIRMWARE(GSP_FILE);
MODULE_FIRMWARE(GSP_BOOT_FILE);
MODULE_FIRMWARE(GSP_BOOT_DESC_FILE);
MODULE_FIRMWARE(GSP_GA100_FILE);
MODULE_FIRMWARE(BOOT_GA100_FILE);
MODULE_FIRMWARE(BOOT_DESC_GA100_FILE);
