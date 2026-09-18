import pathlib, subprocess, tempfile, unittest
ROOT=pathlib.Path(__file__).resolve().parents[1]
class MqttLights(unittest.TestCase):
    def test_state_confirmation_and_expiry(self):
        code = r"""
#include "adapters/mqtt-lights/model.h"
#include <cassert>
int main() {
  using namespace nabla_mqtt_lights;
  model.connect(true);model.update(0,{true,true,true,50},100);
  CompactDraft draft;int sent=0;
  draft.send=[&](int slot,bool on,int level){assert(slot==0);assert(on);assert(level==51);sent++;};
  draft.open(0,"Light",100);draft.move(1);
  assert(draft.level==51 && model.states[0].brightness==50);
  draft.back();assert(!draft.active() && sent==0);
  draft.open(0,"Light",100);draft.move(1);draft.activate(100);
  model.connect(false);draft.activate(101);
  assert(draft.active() && sent==0);draft.back();
  model.connect(true);model.update(0,{true,true,true,50},102);
  draft.open(0,"Light",102);draft.move(1);draft.activate(102);draft.activate(102);
  assert(!draft.active() && sent==1);
  draft.open(0,"Light",103);draft.move(999);assert(draft.level==100);
  draft.move(-999);assert(draft.level==0);draft.back();
  nabla_mqtt_lights::Model m;
  assert(!m.ready(0,100));
  m.connect(true);
  m.update(0,{false,true,true,20},100);
  assert(m.ready(0,101));
  m.dirty=false;
  m.update(0,{false,true,true,20},101);
  assert(!m.dirty && m.seen[0]==101); // heartbeat refreshes freshness, not UI
  m.update(0,{false,true,true,21},101);
  assert(m.dirty); // actual brightness change still renders
  m.dirty=false;
  m.sent(0,true,65,102);
  assert(!m.states[0].on && !m.ready(0,103));
  m.update(0,{true,true,true,25},104);
  assert(m.pending[0]);
  m.update(0,{true,true,true,65},105);
  assert(!m.pending[0] && m.ready(0,106));
  m.dirty=false;
  m.sent(0,true,65,106);
  m.dirty=false;
  m.update(0,{true,true,true,65},107);
  assert(m.dirty && !m.pending[0]); // identical state can confirm a command
  m.tick(90107);
  assert(!m.ready(0,90107));
  m.update(0,{true,true,false,0},90106);
  m.sent(0,false,0,90107);
  m.tick(105107);
  assert(!m.pending[0] && m.states[0].on);
  m.connect(false);
  assert(!m.available(0,105108));
  m.update(99,{true,true,true,100},1);
  for(int i=0;i<300;i++)m.note("bounded event");
  assert(m.log.size()<=900);
}
"""
        with tempfile.TemporaryDirectory() as tmp:
            src=pathlib.Path(tmp)/"check.cpp";src.write_text(code)
            exe=pathlib.Path(tmp)/"check"
            subprocess.run(["g++","-std=c++17","-I",str(ROOT),str(src),"-o",str(exe)],check=True)
            subprocess.run([str(exe)],check=True)
