# Pinned 610.43.03 firmware

`make inputs` downloads the two NVIDIA inputs from
`artifacts-610.43.03.json`, checking SHA-256 before accepting cached files.
`make firmware` extracts the two GSP ELFs, signed GA100 SEC2 Booter bundle,
GA100 generic Falcon loader, and per-family GSP boot images/descriptors.
Every derived file has a pinned SHA-256. Extracted NVIDIA bytes are unchanged.

`generate_v67.py --payload` additionally emits `cmp-signature.bin` as an offline
reference image; normal firmware builds omit it. It is not installed or requested
by the module. Runtime builds the same
63,488-byte payload in C and changes only the requested address/value.
`--header PATH` reproduces the committed `payload_words.h` from the manifest.
There is no optional external `dmem.bin` override.

The SEC2 Booter executes on Falcon and loads the separate GSP boot image.
GA100 uses `gsp_tu10x.bin` with `.fwsignature_ga100`; GA10x uses
`gsp_ga10x.bin` with `.fwsignature_ga10x`. The selected card determines its
GSP assets once, separately from register tables. All new hardware contexts
remain experimental; see [Stage 1 evidence](../docs/STAGE1.md).

Python 3, zstd, and the pinned inputs are sufficient for extraction. The host
C tests additionally require a compiler and OpenSSL development headers.
See [NOTICE.md](NOTICE.md) for provenance and licensing.
