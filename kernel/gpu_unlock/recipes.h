/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef GPU_UNLOCK_RECIPES_H
#define GPU_UNLOCK_RECIPES_H
#include "prep_610.h"

struct gu_reg_write { u32 addr, value; };
struct gu_write_table { const struct gu_reg_write *entries; size_t count; };
struct gu_family_writes {
	const char *name;
	struct gu_write_table privileged, host;
};
struct gu_card {
	u16 device;
	u32 chip; /* PMC_BOOT_0 architecture + implementation */
	bool direct_boot;
	const struct gu_family_writes *writes;
};
#define GU_WRITE_TABLE(t) { .entries = (t), .count = sizeof(t) / sizeof((t)[0]) }

const struct gu_card *gu_card_find(u16 device);
#endif
