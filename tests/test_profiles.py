# Profile geometry and focus tests; checks usable layout bounds across supported viewports.

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
 {"Sensors",0,"","",0}, {"Room",13,"","",0}, {"Temperature",14,"","",0},
 {"Dark mode",4,"","",5}, {"Font",4,"","",6}, {"Borders",4,"","",7}
};
const int count = sizeof(nodes)/sizeof(nodes[0]);
}
int main() {
 using namespace nabla;
 for(int i=2;i<7;++i)assert(i-encoder_anchor(i,4,8,1)==2);
 for(int i=1;i<6;++i)assert(i-encoder_anchor(i,4,8,-1)==1);
 assert(encoder_anchor(7,4,8,1)==4);
 assert(encoder_anchor(1,4,2,1)==0);
 // Detailed multiline content must never leak into compact menu summaries.
 nabla_info::values[2] = "Example - 192.0.2.42";
 nabla_info::details[2] = "Example\nIP: 192.0.2.42\nGateway: 192.0.2.1";
 assert(nabla_info::value(2).find('\n') == std::string::npos);
 assert(nabla_info::detail(2).find("Gateway:") != std::string::npos);
 assert(nabla_info::detail(-1).empty());
 nabla_info::values[3] = "192.0.2.42";
 assert(nabla_info::detail(3) == nabla_info::value(3));

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
   m.focus=2; m.activate();
   assert(m.dark && m.current==4 && m.focus==2);
   assert(row_title(16,m.dark,m.font_family)=="[x] Dark mode");
   m.focus=3; m.activate();
   assert(m.font_family && m.current==4 && m.focus==3);
   assert(row_title(17,m.dark,m.font_family)=="Font: DejaVu Sans");
   m.activate(); assert(m.font_family==0 && m.focus==3);
   assert(row_title(17,m.dark,m.font_family)=="Font: Ubuntu Mono");
   m.focus=4; m.activate(); assert(!m.borders && m.current==4 && m.focus==4);
   assert(row_title(18,m.dark,m.font_family,m.borders)=="[ ] Borders");
   m.activate(); assert(m.borders && m.focus==4);

 }
 // Switching views via the logo returns focus to the previous content item.
 CompactMenu toggle;
 toggle.move(1);
 toggle.touch_option(children(0));
 assert(toggle.readable && toggle.focus==1);
 toggle.move(1);
 assert(toggle.focus==2); // Down continues, rather than wrapping from logo to item zero.
 toggle.move(children(0)-2);
 assert(toggle.focus==children(0) && toggle.root_content_focus==0 && toggle.top==0);
 toggle.activate();
 assert(!toggle.readable && toggle.focus==0);
 toggle.move(children(0));
 assert(toggle.focus==children(0) && toggle.top==0);
 toggle.move(1);
 assert(toggle.focus==0 && toggle.top==0);
 // Landscape encoder target: four rows, with and without its footer.
 for(bool footer : {false,true}) {
   auto g=Geometry::compact(false,footer,160,128,16,4);
   assert(g.width==160 && g.height==128 && g.rows==4);
   assert(g.header+g.rows*g.row_height<=128-g.footer);
 }
 CompactMenu kit; kit.list_rows=4;
 kit.move(7);
 assert(kit.focus==7 && kit.top+kit.rows()>7 && kit.rows()==4);
 kit.activate(); kit.back();
 assert(kit.focus==7);
 kit.readable=true;
 assert(kit.rows()==1);
 for(auto g : {Geometry::regular(480,320,true),Geometry::regular(320,480,true)}) {
   assert(g.content_height() == g.height - 72);
   assert(g.width>0 && g.content_height()>0);
 }
 auto spacious=Geometry::regular(480,320,true);
 auto dense=Geometry::regular(480,320,true,36,true);
 assert(dense.row_height>=20+8); // Keep 20px labels and vertical breathing room.
 assert((dense.content_height()+dense.gap)/(dense.row_height+dense.gap)==7);
 assert((spacious.content_height()+spacious.gap)/(spacious.row_height+spacious.gap)==4);
 assert(dense.header==spacious.header && dense.footer==spacious.footer);
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
            for name in ("navigation.h","forms.h","info.h"):
                shutil.copy(ROOT/"external_components/nabla_navigation"/name, includes/name)
            subprocess.run(["g++","-std=c++17","-I",str(ROOT),"-I",str(ROOT/"navigation"),
                            "-I",str(Path(tmp)),str(cpp),"-o",str(exe)],check=True)
            subprocess.run([str(exe)],check=True)
