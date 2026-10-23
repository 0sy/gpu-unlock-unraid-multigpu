// SPDX-License-Identifier: GPL-2.0-only
#include "payload_610.h"
static void put32(u8 *image, u32 offset, u32 value)
{
	unsigned int i;
	for (i = 0; i < 4; i++)
		image[offset + i] = value >> (8 * i);
}
void gu_payload_fill(u8 *image, u32 address, u32 value)
{
	u32 offset;
	for (offset = 0; offset < GU_PAYLOAD_SIZE; offset += 4)
		put32(image, offset, 0x4a7);
#define GU_PAYLOAD_WORD(offset, value) put32(image, offset, value);
#include "payload_words.h"
#undef GU_PAYLOAD_WORD
	put32(image, 0xf754, value);
	put32(image, 0xf76c, address);
}
