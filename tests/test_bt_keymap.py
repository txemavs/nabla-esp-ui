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

    def test_report_descriptor_and_bounds(self):
        header=pathlib.Path(__file__).resolve().parents[1]/"external_components/nabla_bt_keyboard/report.h"
        source = '#include "'+str(header)+'"\n#include <cassert>\n'+r"""
int main(){
using namespace esphome::nabla_bt_keyboard;
uint8_t descriptor[]={0x05,1,0x09,6,0xA1,1,0x85,1,0x05,7,0x19,0xE0,0x29,0xE7,0x15,0,0x25,1,0x75,1,0x95,8,0x81,2,0x95,1,0x75,8,0x81,1,0x95,6,0x75,8,0x15,0,0x25,0x65,0x19,0,0x29,0x65,0x81,0,0xC0};
auto l=keyboard_layout(descriptor,sizeof(descriptor));assert(l.valid && l.id==1 && l.bytes==8);
uint8_t input[]={1,2,0,4,0,0,0,0,0},output[8]{};
assert(decode_report(l,input,sizeof(input),output));assert(output[0]==2 && output[2]==4);
assert(!decode_report(l,input,3,output));input[0]=2;assert(!decode_report(l,input,sizeof(input),output));
input[0]=1;input[3]=0x52;
assert(decode_input(l,true,true,input,9,output) && output[2]==0x52);
input[0]=2;assert(!decode_input(l,true,true,input,9,output));
assert(!decode_input(l,true,true,input,7,output));
uint8_t boot[]={0,0,0x51,0,0,0,0,0};
assert(decode_input(l,true,true,boot,8,output) && output[2]==0x51);
assert(!decode_input({},true,true,input,9,output));
assert(!keyboard_layout(descriptor,7).valid);
uint8_t bad[]={0x85,255};assert(!keyboard_layout(bad,sizeof(bad)).valid);
}
"""
        with tempfile.TemporaryDirectory() as d:
            cpp=pathlib.Path(d)/"test.cpp";cpp.write_text(source);exe=pathlib.Path(d)/"test"
            subprocess.run(["g++","-std=c++17",str(cpp),"-o",str(exe)],check=True)
            subprocess.run([str(exe)],check=True)
