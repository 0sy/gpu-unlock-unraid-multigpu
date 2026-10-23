/* SPDX-License-Identifier: GPL-2.0-only */
/* Verbatim functions from cmpunlocker 2d10f891, sec2-postbl-plm-ss-cfg.patch. */
static void
_kgspSec2PostblTimingPutU32(NvU8 *pBuffer, NvU32 offset, NvU32 value)
{
    pBuffer[offset + 0] = (NvU8)(value >>  0);
    pBuffer[offset + 1] = (NvU8)(value >>  8);
    pBuffer[offset + 2] = (NvU8)(value >> 16);
    pBuffer[offset + 3] = (NvU8)(value >> 24);
}

static void
_kgspSec2PostblTimingFillPayload(NvU8 *pSignatureVa, NvU64 signatureSize,
                                NvU32 writeAddr, NvU32 writeValue)
{
    NvU64 i;

    for (i = 0; i + sizeof(NvU32) <= signatureSize; i += sizeof(NvU32))
        _kgspSec2PostblTimingPutU32(pSignatureVa, (NvU32)i, SEC2_POSTBL_TIMING_FILL_DWORD);

    _kgspSec2PostblTimingPutU32(pSignatureVa, 0x1100, 0x00000007U);
    _kgspSec2PostblTimingPutU32(pSignatureVa, 0x5b40, 0xc0deca7eU);

    _kgspSec2PostblTimingPutU32(pSignatureVa, 0xf754, writeValue);
    _kgspSec2PostblTimingPutU32(pSignatureVa, 0xf758, 0xc0deca7eU);
    _kgspSec2PostblTimingPutU32(pSignatureVa, 0xf75c, 0x00000cbdU);
    _kgspSec2PostblTimingPutU32(pSignatureVa, 0xf76c, writeAddr);
    _kgspSec2PostblTimingPutU32(pSignatureVa, 0xf774, 0x00001fbdU);
    _kgspSec2PostblTimingPutU32(pSignatureVa, 0xf780, 0x00000000U);
    _kgspSec2PostblTimingPutU32(pSignatureVa, 0xf788, 0x000010aaU);
    _kgspSec2PostblTimingPutU32(pSignatureVa, 0xf78c, 0x0000815aU);
    _kgspSec2PostblTimingPutU32(pSignatureVa, 0xf790, 0x00008e18U);
    _kgspSec2PostblTimingPutU32(pSignatureVa, 0xf794, 0xc0deca7eU);
    _kgspSec2PostblTimingPutU32(pSignatureVa, 0xf798, 0x0000815aU);
    _kgspSec2PostblTimingPutU32(pSignatureVa, 0xf79c, 0x00000000U);
    _kgspSec2PostblTimingPutU32(pSignatureVa, 0xf7a0, 0xc0deca7eU);
    _kgspSec2PostblTimingPutU32(pSignatureVa, 0xf7a4, 0x00001fbdU);
    _kgspSec2PostblTimingPutU32(pSignatureVa, 0xf7b0, 0x0000ffbcU);
    _kgspSec2PostblTimingPutU32(pSignatureVa, 0xf7b8, 0x0000582dU);
    _kgspSec2PostblTimingPutU32(pSignatureVa, 0xf7c4, 0xc0deca7eU);
    _kgspSec2PostblTimingPutU32(pSignatureVa, 0xf7c8, 0x00000cbdU);
    _kgspSec2PostblTimingPutU32(pSignatureVa, 0xf7d8, 0x00000003U);
    _kgspSec2PostblTimingPutU32(pSignatureVa, 0xf7e0, 0x00001fbdU);
    _kgspSec2PostblTimingPutU32(pSignatureVa, 0xf7f4, 0x00000ccbU);
    _kgspSec2PostblTimingPutU32(pSignatureVa, 0xf7f8, 0x00007f2fU);
}

