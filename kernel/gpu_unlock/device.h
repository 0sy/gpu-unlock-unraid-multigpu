/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef GPU_UNLOCK_DEVICE_H
#define GPU_UNLOCK_DEVICE_H

struct pci_dev;
struct gu_card;
struct gpu_unlock_610_fw_desc;

/* Resolved once during enumeration; descriptors outlive boot/resume rounds.
 * The coordinator owns the PCI reference and keeps recipe and firmware
 * selection separate. Round-scoped firmware assets are never stored here. */
struct gu_device {
	struct pci_dev *pdev;
	const struct gu_card *recipe;
	const struct gpu_unlock_610_fw_desc *firmware;
};

#endif
