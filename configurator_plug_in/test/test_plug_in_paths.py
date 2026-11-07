"""Studio system-pack path resolution for the PC MCAL plugin pack.

VCOS Studio (vcos_studio configurator) loads a system pack like this:

* ProjectFileParser.get_plug_in_path joins each module_cfg plug_in_path onto
  the project system_path, then os.path.abspath.
* LoadPlugIn.append_sys_path splits that absolute path on "/components/" and
  unpacks exactly two pieces (prefix, module). A second "/components/" raises
  ValueError, so the pack never finishes loading.

This pack lives at vcos/components/autoc_mcal/configurator_plug_in. The old
Can path "../components/Can" becomes .../components/autoc_mcal/components/Can,
which contains "/components/" twice and does not exist. Can is the sibling
directory ../Can. Upstream BSW paths stay "../../../components/<module>".
"""

from __future__ import annotations

import json
import os
import tempfile
import unittest
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
PACK_DIR = REPO_ROOT / "configurator_plug_in"
CAN_DIR = REPO_ROOT / "Can"
COMPONENTS_MARK = "/components/"


def studio_resolve(system_path: str, relative: str) -> str:
    """Match ProjectFileParser.get_plug_in_path + LoadPlugIn.append_sys_path."""
    return os.path.abspath(os.path.join(system_path, relative)).replace("\\", "/")


def studio_import_name(resolved: str) -> str:
    """Match LoadPlugIn.append_sys_path's two-way split. Extra marks crash."""
    root = COMPONENTS_MARK
    if root not in resolved and "/drivers/" not in resolved:
        raise ValueError(f"System path {resolved} error")
    mark = root if root in resolved else "/drivers/"
    prefix, module = resolved.split(mark)
    if not prefix or not module:
        raise ValueError(f"System path {resolved} error")
    package = "components" if mark == root else "drivers"
    return f"{package}.{module.replace('/', '.')}"


class PcPackPathTests(unittest.TestCase):
    def setUp(self):
        self._tmp = tempfile.TemporaryDirectory()
        root = Path(self._tmp.name)
        pack_parent = root / "vcos" / "components" / "autoc_mcal"
        pack_parent.mkdir(parents=True)
        self.system_path = pack_parent / "configurator_plug_in"
        self.can_link = pack_parent / "Can"
        self.system_path.symlink_to(PACK_DIR, target_is_directory=True)
        self.can_link.symlink_to(CAN_DIR, target_is_directory=True)
        cfg_path = self.system_path / "components_plug_in_cfg.json"
        self.cfg = json.loads(cfg_path.read_text(encoding="utf-8"))

    def tearDown(self):
        self._tmp.cleanup()

    def test_can_path_is_sibling_and_importable(self):
        relative = self.cfg["module_cfg"]["Can"]["plug_in_path"]
        self.assertEqual(relative, "../Can")
        resolved = studio_resolve(str(self.system_path), relative)
        self.assertTrue(resolved.endswith("/components/autoc_mcal/Can"), resolved)
        self.assertEqual(resolved.count(COMPONENTS_MARK), 1, resolved)
        self.assertEqual(studio_import_name(resolved), "components.autoc_mcal.Can")
        self.assertTrue(os.path.isdir(os.path.join(resolved, "config_plug_in", "base_config", "bswmd", "comm_bswmd")))
        self.assertTrue(os.path.isdir(os.path.join(resolved, "config_plug_in", "generator")))

    def test_stale_components_can_path_crashes_studio_split(self):
        resolved = studio_resolve(str(self.system_path), "../components/Can")
        self.assertIn("/components/autoc_mcal/components/Can", resolved)
        self.assertFalse(os.path.isdir(resolved))
        self.assertGreaterEqual(resolved.count(COMPONENTS_MARK), 2)
        with self.assertRaises(ValueError):
            studio_import_name(resolved)

    def test_every_registered_path_splits_once(self):
        for name, info in self.cfg["module_cfg"].items():
            relative = info.get("plug_in_path")
            self.assertTrue(relative, name)
            resolved = studio_resolve(str(self.system_path), relative)
            marks = resolved.count(COMPONENTS_MARK) + resolved.count("/drivers/")
            self.assertEqual(marks, 1, f"{name}: {relative} -> {resolved}")
            studio_import_name(resolved)

    def test_bsw_paths_stay_outside_this_pack(self):
        os_path = studio_resolve(str(self.system_path), self.cfg["module_cfg"]["Os"]["plug_in_path"])
        canif_path = studio_resolve(str(self.system_path), self.cfg["module_cfg"]["CanIf"]["plug_in_path"])
        self.assertTrue(os_path.endswith("/components/rt_framework"), os_path)
        self.assertTrue(canif_path.endswith("/components/communication/net_dev/canif"), canif_path)
        self.assertNotIn("/autoc_mcal/", os_path)
        self.assertNotIn("/autoc_mcal/", canif_path)

    def test_pc_common_publishes_os_priority_limit(self):
        limits = self.cfg["validate"]["ignore_option"]["PC_Common"]["Os"]
        self.assertEqual(limits["max_interrupt_priority"], 255)
        self.assertIn("PC_Common", self.cfg["target"])


if __name__ == "__main__":
    unittest.main()
