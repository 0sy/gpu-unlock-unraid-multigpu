/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef GPU_UNLOCK_FIRMWARE_610_H
#define GPU_UNLOCK_FIRMWARE_610_H

#include "prep_610.h"
#include "device.h"

struct firmware;
/* Immutable family format/launch descriptor, selected only for a known recipe. */
struct gpu_unlock_610_fw_desc {
	const char *gsp, *boot, *desc, *signature_section;
	u32 boot_size;
	bool direct;
};

const struct gpu_unlock_610_fw_desc *
gpu_unlock_610_select_firmware(const struct gu_card *recipe);

/* Immutable after preparation, including decoded metadata. Owns gsp and boot. */
struct gpu_unlock_610_firmware {
	const struct firmware *gsp, *boot;
	struct gpu_unlock_span image_span, sig_span;
	struct gpu_unlock_boot_desc boot_desc;
	struct gpu_unlock_radix_plan radix;
	bool prepared;
	int error;
};

/* One coordination round. Common images are shared by both firmware families. */
struct gpu_unlock_610_shared {
	struct gpu_unlock_610_firmware ga100, ga10x;
	const struct firmware *booter, *loader;
	struct gpu_unlock_booter_hdr booter_hdr;
};

/* Serial preparation only; no workers may run until all calls have returned.
 * Cache failures for this round without invalidating another family's assets.
 * A fresh round starts after release_shared(), including on resume. */
int gpu_unlock_610_load_shared(const struct gu_device *device, struct gpu_unlock_610_shared *shared);
void gpu_unlock_610_release_shared(struct gpu_unlock_610_shared *shared);

#endif
