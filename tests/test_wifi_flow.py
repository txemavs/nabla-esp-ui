# Shared Wi-Fi flow tests; checks scanning, editing and cancellation independently of physical networking.

from pathlib import Path
import subprocess
import tempfile
import unittest
ROOT=Path(__file__).resolve().parents[1]
class WifiFlowTests(unittest.TestCase):
    def test_drafts_and_async_lifecycle(self):
        source=r"""
#include "compact_wifi.h"
#include <cassert>
int main(){
 using namespace nabla_forms;
 TextDraft d;d.limit=4;
 assert(d.append("á"));assert(d.append("ñ"));assert(!d.append("a"));
 d.erase();assert(d.value=="á");d.erase();assert(d.value.empty());

 // Reject malformed, embedded-NUL, surrogate and oversized Unicode input.
 assert(!d.append(std::string("\0",1)));assert(!d.append("\xC0\xAF"));
 assert(!d.append("\xED\xA0\x80"));assert(!d.append("\xF4\x90\x80\x80"));
 assert(d.append("\xF0\x9F\x98\x80"));d.erase();assert(d.value.empty());
 WifiFlow f;
 assert(!f.connect(0));assert(f.error==Error::SSID);
 f.ssid="demo";assert(!f.connect(0));assert(f.error==Error::PASSWORD);
 f.password="12345678";assert(f.connect(0));assert(f.password.empty());
 auto request=f.revision;assert(!f.connect(1));assert(f.revision==request);
 f.tick(999);assert(f.stage==Stage::CONNECTING);
 f.cancel();f.tick(2000);assert(f.stage==Stage::EDIT);
 f.scan(3000);f.tick(3699);assert(f.stage==Stage::SCANNING);
 f.tick(3700);assert(f.stage==Stage::RESULTS);assert(f.choose(1));
 assert(f.open && f.password.empty());assert(f.connect(4000));
 f.tick(5000);assert(f.stage==Stage::SUCCESS);
 f.scan(6000,true);f.tick(6700);assert(f.results()==0);assert(!f.choose(0));
 f.scan(7000);f.tick(7700);f.choose(2);f.password="12345678";
 assert(f.connect(8000));f.tick(9000);assert(f.stage==Stage::FAILURE);
 f.cancel();f.scan(10000);f.tick(10700);f.choose(0);f.password="12345678";
 assert(f.connect(11000));f.tick(12000);assert(f.stage==Stage::SUCCESS);
 f.clear();assert(f.ssid.empty() && f.password.empty() && f.stage==Stage::EDIT);
 f.scan(0xFFFFFF00);f.tick(0x00000200);assert(f.stage==Stage::RESULTS);

 // Duplicate SSID/security collapses to strongest signal; open/protected remain distinct.
 f.scan(0);f.tick(700);assert(f.results()==6);
 assert(std::string(f.result(0).ssid)=="Nabla Demo" && f.result(0).rssi==-35);
 assert(f.result(5).open && !f.result(0).open);
 assert(std::string(f.result(4).ssid).size()==32);
 auto rev=f.revision;f.scan(1000);assert(!f.scan(1001));assert(f.revision==rev+1);
 f.cancel();f.tick(2000);assert(f.stage==Stage::EDIT);
 f.scan(3000,ScanMode::ERROR);f.tick(3700);assert(f.stage==Stage::SCAN_ERROR && !f.choose(0));
 f.scan(4000);f.tick(4700);assert(f.choose(4));
 f.password="12345678";assert(f.connect(5000));auto accepted=f.accepted;
 // Mutable draft cannot alter an in-flight operation's result.
 f.ssid="edited";f.tick(6000);assert(f.stage==Stage::SUCCESS);
 assert(!f.connect(6001) && f.accepted==accepted);
 f.cancel();f.ssid=std::string(33,'a');f.password="12345678";assert(!f.connect(7000));
 f.ssid=std::string(32,'a');assert(f.connect(7100));
 f.clear();f.tick(9000);assert(f.stage==Stage::EDIT);
 CompactWifi e;e.flow.ssid="original";e.edit(false);
 e.draft.value="changed";assert(!e.back());assert(e.flow.ssid=="original");
 e.edit(false);e.draft.value="new";e.key=0;e.activate(0);assert(e.flow.ssid=="new");
 e.flow.password="private";e.clear();assert(e.flow.password.empty() && e.draft.value.empty());
 // Sequential selector reaches cancel without a physical Escape key.
 e.edit(true);e.move(2);e.activate(0);assert(!e.editing);
 e.focus=7;assert(e.activate(0));
}
"""
        with tempfile.TemporaryDirectory() as tmp:
            cpp=Path(tmp)/"wifi.cpp";exe=Path(tmp)/"wifi"
            cpp.write_text(source)
            subprocess.run(["g++","-std=c++17","-I",str(ROOT/"components/forms"),
                            "-I",str(ROOT/"navigation"),str(cpp),"-o",str(exe)],check=True)
            subprocess.run([str(exe)],check=True)
