from pathlib import Path
import subprocess
import tempfile
import shutil
import unittest
ROOT = Path(__file__).resolve().parents[1]

class ProfileTests(unittest.TestCase):
    def test_geometry_focus_and_route_traces(self):
        source = r"""
#include "components/compact-shell/model.h"
#include "profiles/geometry.h"
#include <cassert>
namespace nabla {
const Node nodes[] = {
 {"Home",-1,"","",0},
 {"Settings",0,"","",0}, {"Connections",1,"","",0}, {"Wi-Fi",2,"","",3},
 {"Appearance",1,"","",0}, {"Dark",4,"","",1}, {"Light",4,"","",2},
 {"Communications",0,"","",0}, {"Photos",0,"","",0}, {"Music",0,"","",0},
 {"Cameras",0,"","",0}, {"Weather",0,"","",0}, {"Lights",0,"","",0},
 {"Sensors",0,"","",0}, {"Room",13,"","",0}, {"Temperature",14,"","",0}
};
const int count = sizeof(nodes)/sizeof(nodes[0]);
}
int main() {
 using namespace nabla;
 for (bool readable : {false,true}) {
   for (bool footer : {false,true}) {
     auto g = Geometry::compact(readable,footer);
     assert(g.width==128 && g.height==64);
     assert(g.header + g.rows*g.row_height <= 64-g.footer);
     assert(g.rows == (readable?1:3));
   }
   CompactMenu encoder, touch;
   encoder.readable = touch.readable = readable;
   // Identical content routes for sequential and touch input at every root slot.
   for(int i=0;i<8;i++) {
     encoder.home(); touch.home();
     encoder.focus=0; encoder.move(i); encoder.activate();
     touch.touch_option(i);
     assert(encoder.current==touch.current);
     assert(encoder.focus==0);
   }
   CompactMenu m; m.readable=readable;
   m.move(7);
   assert(m.focus==7 && m.top<=7 && m.top+m.rows()>7);
   m.activate(); assert(m.current==13);
   m.activate(); assert(m.current==14);
   m.activate(); assert(m.current==15);
   m.activate(); assert(m.current==14); // on-screen Back on a leaf
   m.back(); m.back(); assert(m.current==0 && m.focus==7);
   // Re-enter after leaving through logo: first child, not saved toolbar focus.
   m.focus=0; m.activate(); assert(m.current==1);
   m.focus=children(m.current); m.activate(); assert(m.current==0);
   m.activate(); assert(m.current==1 && m.focus==0);
   m.activate(); m.activate(); assert(m.current==3);
   m.home(); assert(m.current==0);
   // Toggle density at the root without changing route.
   m.focus=8; m.activate(); assert(m.current==0 && m.readable!=readable);
   // Theme action returns to its parent and does not strand focus.
   m.open(6); assert(!m.dark && m.current==4 && m.focus<children(4));
 }
 for(auto g : {Geometry::regular(480,320,true),Geometry::regular(320,480,true)}) {
   assert(g.content_height() == g.height - 72);
   assert(g.width>0 && g.content_height()>0);
 }
 assert(content_focus(4,4,false)==0);
 assert(content_focus(2,4,false)==2);
 assert(content_focus(2,4,true)==0);
 assert(wrap_focus(0,-1,9)==8);
}
"""
        with tempfile.TemporaryDirectory() as tmp:
            cpp=Path(tmp)/"profiles.cpp"; exe=Path(tmp)/"profiles"
            cpp.write_text(source)
            includes=Path(tmp)/"esphome/components/nabla_navigation"
            includes.mkdir(parents=True)
            shutil.copy(ROOT/"external_components/nabla_navigation/navigation.h", includes/"navigation.h")
            subprocess.run(["g++","-std=c++17","-I",str(ROOT),"-I",str(ROOT/"navigation"),
                            "-I",str(Path(tmp)),str(cpp),"-o",str(exe)],check=True)
            subprocess.run([str(exe)],check=True)
