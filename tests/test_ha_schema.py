import importlib.util
from pathlib import Path
import unittest
import esphome.config_validation as cv
ROOT=Path(__file__).resolve().parents[1]
spec=importlib.util.spec_from_file_location("nabla_ha",ROOT/"external_components/nabla_ha/__init__.py")
module=importlib.util.module_from_spec(spec)
spec.loader.exec_module(module)
class SchemaTests(unittest.TestCase):
    def test_https_origins_only(self):
        self.assertEqual(module.url("https://ha.example.org/"),"https://ha.example.org")
        self.assertEqual(module.url("https://ha.example.org:8123"),"https://ha.example.org:8123")
        for value in ["http://ha.example.org","https://user:pass@ha.example.org","https://ha.example.org/api","https://ha.example.org?token=x"]:
            with self.assertRaises(cv.Invalid): module.url(value)
    def test_explicit_light_switch_allowlist(self):
        self.assertEqual(module.entity("light.desk"),"light.desk")
        self.assertEqual(module.entity("switch.lamp"),"switch.lamp")
        for value in ["lock.door","light../services","light.*","light.desk?x=y","light.Desk"]:
            with self.assertRaises(cv.Invalid):module.entity(value)
