import copy
import importlib.util
from pathlib import Path
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location("catalog", ROOT / "external_components/nabla_navigation/catalog.py")
catalog = importlib.util.module_from_spec(spec)
spec.loader.exec_module(catalog)

class CatalogTests(unittest.TestCase):
    def tree(self):
        return {"key": "home", "title": "Home", "children": [
            {"key": "settings", "title": "Settings", "icon": "S", "children": [
                {"key": "wifi", "title": 'Wi-Fi "guest"', "detail": "line1\nline2"},
                {"key": "dark", "title": "Dark", "action": "dark"}]},
            {"key": "sensors", "title": "Sensores", "icon": "T"}]}

    def test_nested_parent_and_order(self):
        nodes = catalog.flatten(self.tree())
        self.assertEqual([n["parent"] for n in nodes], [-1, 0, 1, 1, 0])
        self.assertEqual(nodes[3]["action"], "dark")

    def test_tile_colors(self):
        t = self.tree()
        t["children"][0].update(bg_dark=0x12345A, bg_light=0x9ABCE3)
        node = catalog.flatten(t)[1]
        self.assertEqual((node["bg_dark"], node["bg_light"]), (0x12345A, 0x9ABCE3))
        for invalid in (-1, 0x1000000, True, "blue"):
            t["children"][0]["bg_dark"] = invalid
            with self.assertRaises(ValueError):
                catalog.flatten(t)

    def test_invalid_configurations(self):
        cases = []
        t = self.tree(); t["children"][1]["key"] = "settings"; cases.append(t)
        t = self.tree(); t["children"] *= 5; cases.append(t)
        t = self.tree(); t["children"][0]["icon"] = ""; cases.append(t)
        t = self.tree(); t["children"][0]["action"] = "dark"; cases.append(t)
        t = self.tree(); t["action"] = "invalid"; cases.append(t)
        t = self.tree(); t["title"] = ""; cases.append(t)
        t = self.tree(); t["parent"] = "missing"; cases.append(t)
        for tree in cases:
            with self.subTest(tree=tree), self.assertRaises(ValueError):
                catalog.flatten(tree)

    def test_generated_cpp_escaping_and_traversal(self):
        header = ROOT / "external_components/nabla_navigation/navigation.h"
        source = '#include "' + str(header) + '"\n#include <cassert>\n'
        source += catalog.emit(self.tree())
        source += '''
int main() {
  assert(nabla::children(0) == 2);
  assert(nabla::child(0, 1) == 4);
  assert(nabla::child(1, 2) == -1);
  assert(nabla::nodes[3].action == 1);
  assert(nabla::path(2).find("guest") != std::string::npos);
  assert(std::string(nabla::nodes[2].detail).find(char(10)) != std::string::npos);
  assert(nabla::path(-1).empty());
}
'''
        with tempfile.TemporaryDirectory() as directory:
            cpp = Path(directory) / "test.cpp"; exe = Path(directory) / "test"
            cpp.write_text(source)
            subprocess.run(["g++", "-std=c++17", str(cpp), "-o", str(exe)], check=True)
            subprocess.run([str(exe)], check=True)

if __name__ == "__main__":
    unittest.main()
