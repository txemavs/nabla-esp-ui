"""Host checks for bounded printable HID translation."""
import pathlib, subprocess, tempfile, unittest
class KeymapTest(unittest.TestCase):
    def test_common_and_control_keys(self):
        header = pathlib.Path(__file__).resolve().parents[1] / "external_components/nabla_bt_keyboard/keymap.h"
        source = '#include "' + str(header) + '"\n#include <cassert>\nint main(){using esphome::nabla_bt_keyboard::latin_key; assert(latin_key(4,0)==\'a\'); assert(latin_key(29,2)==\'Z\'); assert(latin_key(4,1)==0); assert(latin_key(4,0x40)==0); assert(latin_key(39,0)==\'0\'); assert(latin_key(0x52,0)==0); assert(latin_key(255,0)==0);}'
        with tempfile.TemporaryDirectory() as d:
            cpp=pathlib.Path(d)/"test.cpp"; cpp.write_text(source)
            exe=pathlib.Path(d)/"test"
            subprocess.run(["g++","-std=c++17",str(cpp),"-o",str(exe)],check=True)
            subprocess.run([str(exe)],check=True)
