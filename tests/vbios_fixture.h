/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef GPU_UNLOCK_TEST_VBIOS_FIXTURE_H
#define GPU_UNLOCK_TEST_VBIOS_FIXTURE_H
/* Synthetic, redistributable ROM; no board firmware contents. */
static void vbios_w32(u8 *p, u32 v)
{
    for (int i = 0; i < 4; i++) p[i] = v >> (8 * i);
}
static void vbios_fixture(u8 *rom, int stride)
{
	unsigned int sum = 0;

	memset(rom, 0, 4096);
	rom[0] = 0x55; rom[1] = 0xaa; rom[0x18] = 32;
	memcpy(rom + 32, "PCIR", 4);
	rom[42] = 24; rom[48] = 8; rom[53] = 0x80;
	rom[64] = 0xff; rom[65] = 0xb8; memcpy(rom + 66, "BIT", 3);
	rom[71] = 1; rom[72] = 12; rom[73] = stride; rom[74] = 1;
	for (int i = 64; i < 75; i++)
		sum += rom[i];
	rom[75] = -sum;
	rom[76] = 0x70; rom[77] = 2; rom[78] = 4; vbios_w32(rom + 80, 100);
	vbios_w32(rom + 100, 128);
	rom[128] = 1; rom[129] = 6; rom[130] = 6; rom[131] = 2;
	/* FWSEC need not be the first entry. */
	rom[134] = 0x45; rom[140] = 0x85; vbios_w32(rom + 142, 256);
	vbios_w32(rom + 256, (428u << 16) | 0x301);
	vbios_w32(rom + 260, 1280); vbios_w32(rom + 264, 512); vbios_w32(rom + 268, 0);
	vbios_w32(rom + 276, 256); vbios_w32(rom + 288, 1024);
	rom[292] = 1; rom[294] = 1; rom[295] = 1; rom[296] = 1;
}

/* GA100 v2 descriptor; signed code/data contents are synthetic. */
static void vbios_fixture_v2(u8 *rom)
{
	vbios_fixture(rom, 6);
	memset(rom + 256, 0, 1536);
	vbios_w32(rom + 256, (60u << 16) | 0x201);
	vbios_w32(rom + 260, 1280);
	vbios_w32(rom + 264, 1280);
	vbios_w32(rom + 280, 256);
	vbios_w32(rom + 288, 0);
	vbios_w32(rom + 292, 256);
	vbios_w32(rom + 296, 256);
	vbios_w32(rom + 304, 1024);
	u8 *dmem = rom + 316 + 256;
	dmem[0] = 1; dmem[1] = 4; dmem[2] = 8; dmem[3] = 2;
	vbios_w32(dmem + 4, 4); vbios_w32(dmem + 8, 32);
	dmem[36] = 3; dmem[38] = 64;
	vbios_w32(dmem + 40, 128); vbios_w32(dmem + 44, 44);
}

#endif
