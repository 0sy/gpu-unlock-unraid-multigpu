/* SPDX-License-Identifier: GPL-2.0-only */
#include <openssl/sha.h>
#define SHA256_DIGEST_SIZE SHA256_DIGEST_LENGTH
static inline void sha256(const u8 *data, size_t len, u8 *out) { SHA256(data, len, out); }
