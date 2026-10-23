// SPDX-License-Identifier: GPL-2.0-only
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "payload_610.h"
#include "recipes.h"
#include "stage1_tables.h"
typedef u8 NvU8;
typedef u32 NvU32;
typedef u64 NvU64;
#define SEC2_POSTBL_TIMING_FILL_DWORD 0x4a7
#include "cmp_payload_reference.h"

int main(int argc, char **argv)
{
	u8 actual[GU_PAYLOAD_SIZE], expected[GU_PAYLOAD_SIZE];
	if (argc == 3) {
		gu_payload_fill(actual, strtoul(argv[1], NULL, 0), strtoul(argv[2], NULL, 0));
		return fwrite(actual, 1, sizeof(actual), stdout) == sizeof(actual) ? 0 : 1;
	}
	const unsigned int counts[] = { 2, 3, 12 };
	size_t i, j;
	for (i = 0; i < sizeof(counts) / sizeof(counts[0]); i++) {
		const struct gu_write_table *table = test_stage1_table(counts[i]);
		assert(table && table->count == counts[i]);
		for (j = 0; j < table->count; j++) {
			const struct gu_reg_write *e = &table->entries[j];
			gu_payload_fill(actual, e->addr, e->value);
			_kgspSec2PostblTimingFillPayload(expected, sizeof(expected), e->addr, e->value);
			assert(!memcmp(actual, expected, sizeof(actual)));
		}
	}
	assert(!gu_card_find(0xffff) && !gu_card_find(0x2531));
	assert(!test_stage1_table(0) && !test_stage1_table(7));
	const u16 ids[] = { 0x20c2, 0x2082, 0x2204, 0x2414, 0x2484, 0x2504 };
	const u32 chips[] = { 0x170, 0x170, 0x172, 0x173, 0x174, 0x176 };
	for (i = 0; i < sizeof(ids) / sizeof(ids[0]); i++) {
		const struct gu_card *card = gu_card_find(ids[i]);
		assert(card && card->chip == chips[i]);
		assert(card->direct_boot == (i < 2));
		assert(card->writes->privileged.count == (i < 2 ? 12u : 2u) && card->writes->host.count == 2);
		assert(card->writes->privileged.entries[0].addr == 0x1fa7cc);
		assert(card->writes->privileged.entries[0].value == 0xfffff0ff);
	}
	puts("complete payload byte parity against pinned reference, tables and family mappings PASS");
}
