// SPDX-License-Identifier: GPL-2.0-only
#include "recipes.h"
#include "registers.h"

/* cmpunlocker 2d10f891, sec2-postbl-plm-ss-cfg.patch, plmTable.
 * Order and WPR_CFG's source-control bits are significant. No geometry,
 * link retraining, ECC or GPC/TPC configuration is performed in Stage 1.
 */
/* Only the masks needed by the GA10x compute recipe. */
static const struct gu_reg_write ga10x_plm[] = {
	{ GU_WPR_CFG_PLM_OFFSET, 0xfffff0ff },
	{ GU_FEAT_PLM_OFFSET,    0xffffffff },
};
static const struct gu_reg_write ga100_plm[] = {
	{ GU_WPR_CFG_PLM_OFFSET,   0xfffff0ff },
	{ GU_FBPA_PLM_OFFSET,      0xffffffff },
	{ GU_WPR_PLM_OFFSET,       0xffffffff },
	{ GU_FEAT_PLM_OFFSET,      0xffffffff },
	{ GU_XVE_PLM_OFFSET,       0xffffffff },
	{ GU_XVE_B_PLM_OFFSET,     0xffffffff },
	{ GU_XVE_C_PLM_OFFSET,     0xffffffff },
	{ GU_FEAT2_PLM_OFFSET,     0xffffffff },
	{ GU_OPT_PLM_OFFSET,       0xffffffff },
	{ GU_RECONFIG_PLM_OFFSET,  0xffffffff },
	{ GU_PJTAG_PLM_OFFSET,     0xffffffff },
	{ GU_PJTAG_SEC_PLM_OFFSET, 0xffffffff },
};
static const struct gu_reg_write selectors[] = {
	{ GU_FEAT_SS0_OFFSET, GU_SS0_UNLOCK }, { GU_FEAT_SS1_OFFSET, GU_SS1_UNLOCK },
};
static const struct gu_family_writes ga100 = {
	.name = "GA100",
	.privileged = GU_WRITE_TABLE(ga100_plm), .host = GU_WRITE_TABLE(selectors),
};
static const struct gu_family_writes ga10x = {
	.name = "GA102/GA103/GA104/GA106",
	.privileged = GU_WRITE_TABLE(ga10x_plm), .host = GU_WRITE_TABLE(selectors),
};
#define GA100(id) { id, 0x170, true, &ga100 }
#define GA102(id) { id, 0x172, false, &ga10x }
#define GA103(id) { id, 0x173, false, &ga10x }
#define GA104(id) { id, 0x174, false, &ga10x }
#define GA106(id) { id, 0x176, false, &ga10x }
static const struct gu_card cards[] = {
	GA100(0x20c2), GA100(0x2082),
	GA102(0x2200), GA102(0x2203), GA102(0x2204), GA102(0x2205),
	GA102(0x2206), GA102(0x2207), GA102(0x2208), GA102(0x220a),
	GA102(0x220d), GA102(0x2216), GA102(0x222b), GA102(0x222f),
	GA104(0x2480), GA104(0x2482), GA104(0x2483), GA104(0x2484),
	GA104(0x2486), GA104(0x2487), GA104(0x2488), GA104(0x2489),
	GA104(0x248a), GA104(0x248c), GA104(0x248d), GA104(0x248e),
	GA104(0x249c), GA104(0x249d), GA104(0x249f), GA104(0x24a0),
	GA104(0x24a4), GA104(0x24ac), GA104(0x24ad), GA104(0x24af),
	GA104(0x24bf), GA104(0x24c0), GA104(0x24c7), GA104(0x24c8),
	GA104(0x24c9), GA104(0x24dc), GA104(0x24dd), GA104(0x24df),
	GA104(0x24e0), GA103(0x2414),
	GA106(0x2501), GA106(0x2503), GA106(0x2504), GA106(0x2505),
	GA106(0x2507), GA106(0x2508), GA106(0x2509), GA106(0x252f),
	GA106(0x2544),
};
const struct gu_card *gu_card_find(u16 device)
{
	size_t i;
	for (i = 0; i < sizeof(cards) / sizeof(cards[0]); i++)
		if (cards[i].device == device)
			return &cards[i];
	return NULL;
}
