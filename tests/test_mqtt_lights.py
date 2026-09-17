import pathlib, subprocess, tempfile, unittest
ROOT=pathlib.Path(__file__).resolve().parents[1]
class MqttLights(unittest.TestCase):
    def test_state_confirmation_and_expiry(self):
        code = r"""
#include "adapters/mqtt-lights/model.h"
#include <cassert>
int main() {
  nabla_mqtt_lights::Model m;
  assert(!m.ready(0,100));
  m.connect(true);
  m.update(0,{false,true,true,20},100);
  assert(m.ready(0,101));
  m.sent(0,true,65,102);
  assert(!m.states[0].on && !m.ready(0,103));
  m.update(0,{true,true,true,25},104);
  assert(m.pending[0]);
  m.update(0,{true,true,true,65},105);
  assert(!m.pending[0] && m.ready(0,106));
  m.tick(90105);
  assert(!m.ready(0,90105));
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
