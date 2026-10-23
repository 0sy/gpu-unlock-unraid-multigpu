/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef GPU_UNLOCK_TEST_STAGE1_TABLES_H
#define GPU_UNLOCK_TEST_STAGE1_TABLES_H
#include "recipes.h"

/* Intermediate bring-up fixture. Production selects recipes by device ID. */
static const struct gu_reg_write test_three_plm[] = {
	{ 0x001fa7cc, 0xfffff0ff }, { 0x00823804, 0xffffffff },
	{ 0x009a0148, 0xffffffff },
};
static const struct gu_write_table test_three_table = GU_WRITE_TABLE(test_three_plm);

static const struct gu_write_table *test_stage1_table(unsigned int count)
{
	switch (count) {
	case 2: return &gu_card_find(0x2204)->writes->privileged;
	case 3: return &test_three_table;
	case 12: return &gu_card_find(0x20c2)->writes->privileged;
	default: return NULL;
	}
}
#endif
