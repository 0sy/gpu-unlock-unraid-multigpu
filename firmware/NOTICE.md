# Firmware provenance and distribution scope

`gsp_ga10x.bin` and `gsp_tu10x.bin` are extracted byte-for-byte from the checksummed NVIDIA
610.43.03 package in `artifacts-610.43.03.json`. The Booter and GSP boot images
are decoded unchanged from the pinned 610.43.03 source archive. The extraction
also preserves that package's complete `usr/share/licenses/nvidia-utils/LICENSE`
as `NVIDIA-LICENSE`. It is installed with the firmware package.

The source archive's open-module licensing does **not** license NVIDIA firmware.
The binary package contains the NVIDIA Driver License Agreement. Its section
1.1(d) addresses distribution for open source operating-system kernels with
unchanged binary files and inclusion of the agreement. Additional limitations
apply, including NVIDIA-platform use. Personal local packaging is the initial
delivery target. Review the full pinned agreement and the separately generated
payload before any public distribution; redistribution approval is not claimed.

The payload constants and ordered diagnostic PLM group are adapted from
amoghmunikote/cmpunlocker at commit
`2d10f891a58824f0591c3332c4eccf37ff36b086`, patch
`driver/patches/sec2-postbl-plm-ss-cfg.patch`, function
`_kgspSec2PostblTimingFillPayload`. Its repository license is GPL version 2,
preserved in `licenses/GPL-2.0.txt`. The C runtime and offline generator
reproduce its built-in template, including the full exit tail. The generated
payload is separate from the unchanged NVIDIA firmware.

NVIDIA source files inspected during the loader audit carry their own MIT
copyright notices. The minimum helper's metadata layout and Falcon operations
follow those source contracts; `licenses/NVIDIA-MIT.txt` preserves the notice.
The inspected EFI reference's notice is in `licenses/CMP40HX-MIT.txt`.
The payload reference targets GA100 CMP 170HX. It does not establish GA10x
firmware acceptance, standalone multiwrite, final FLR retention or stock-driver
handover. Those require the Stage 1 hardware experiments.
