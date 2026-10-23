# SPDX-License-Identifier: GPL-2.0-only
"""Host checks for the retained module components."""
from pathlib import Path
import os
import sys
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "firmware"))
import generate_v67
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[1]
KERNEL = ROOT / "kernel/gpu_unlock"
# Generated or installed firmware lets the adapter run the whole unlock
# against mocked hardware.
ASSETS = next((path for path in (Path(os.environ.get("GPU_UNLOCK_ASSETS", "")),
                                 Path("/usr/lib/firmware/gpu-unlock/610.43.03"))
               if all((path / name).is_file() for name in (
                   "gsp_ga10x.bin", "gsp_tu10x.bin", "booter_load_ga100.bin",
                   "falcon_loader.bin", "gsp_boot_ga100.bin", "gsp_boot_desc_ga100.bin",
                   "gsp_boot_ga102.bin", "gsp_boot_desc_ga102.bin"))), None)

CASES = {
    "gfw_eval": ["gfw_eval.c"],
    "layout_610": ["layout_610.c"],
    "prep_610": ["layout_610.c", "prep_610.c"],
    "vbios_610": ["vbios_610.c"],
    "unlock_adapter": ["gfw_eval.c", "prep_610.c", "layout_610.c", "vbios_610.c", "recipes.c", "payload_610.c", "falcon.c"],
    "main": ["recipes.c"],
    "payload_610": ["recipes.c", "payload_610.c"],
}
# Tests that include a kernel source directly build it against a shim.
SHIMS = {
    "unlock_adapter": ["-I", str(ROOT / "tests/unlock_shim")],
    # The kernel build does not warn on sign-compare; match it for main.c.
    "main": ["-I", str(ROOT / "tests/main_shim"), "-pthread", "-Wno-sign-compare"],
}


class ComponentTests(unittest.TestCase):
    def test_components(self):
        with tempfile.TemporaryDirectory() as directory:
            for name, sources in CASES.items():
                with self.subTest(name=name):
                    output = Path(directory) / name
                    shim = SHIMS.get(name, [])
                    command = [os.environ.get("CC", "cc"), "-std=c11", "-Wall",
                               "-Wextra", "-Werror", "-Wno-unused-parameter",
                               "-I", str(KERNEL), *shim,
                               *(str(KERNEL / source) for source in sources),
                               str(ROOT / "tests" / f"test_{name}.c"),
                               "-o", str(output), *(["-lcrypto"] if name == "unlock_adapter" else [])]
                    subprocess.run(command, check=True)
                    args = [str(output)]
                    if name == "unlock_adapter" and ASSETS:
                        args += ["-", str(ASSETS)]
                    if name == "payload_610":
                        for address, value in ((0x1fa7cc, 0xfffff0ff), (0x823804, 0xffffffff), (0x9a0148, 0xffffffff)):
                            data = subprocess.check_output([str(output), hex(address), hex(value)])
                            self.assertEqual(data, generate_v67.build_payload(address, value))
                    result = subprocess.run(args, capture_output=True, text=True)
                    self.assertEqual(result.returncode, 0,
                                     (result.stdout + result.stderr)[-4000:])
