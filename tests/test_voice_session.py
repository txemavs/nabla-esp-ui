"""Host checks for voice cancellation, bounds and half-duplex state transitions."""
import pathlib
import subprocess
import tempfile
import unittest

class VoiceSessionTest(unittest.TestCase):
    def test_session_and_audio_bounds(self):
        header = pathlib.Path(__file__).resolve().parents[1] / "components/voice-session/voice_session.h"
        source = '#include "' + str(header) + '"\n#include <cassert>\n' + r"""
int main() {
  using namespace nabla::voice;
  Session s; int16_t input[Session::capacity]{}, out[Session::capacity]{};
  input[0]=123; assert(!s.start()); s.set_ready(true); assert(s.start());
  assert(!s.begin_response()); assert(s.push(input,Session::capacity));
  assert(!s.push(input,1)); assert(!s.finish_capture());
  assert(s.pop(out,Session::capacity)==Session::capacity && out[0]==123);
  assert(s.finish_capture()); assert(!s.push(input,1)); assert(s.begin_response());
  assert(s.push(input,1)); assert(!s.finish_response()); s.mute(true);
  assert(s.state()==State::idle && s.pop(out,1)==0 && !s.start());
  s.mute(false); assert(s.start()); assert(s.push(input,1)); s.set_ready(false);
  assert(s.state()==State::idle && s.pop(out,1)==0);
  s.set_ready(true); assert(s.start()); assert(s.finish_capture());
  assert(s.begin_response()); assert(s.finish_response());
}
"""
        with tempfile.TemporaryDirectory() as directory:
            cpp = pathlib.Path(directory) / "test.cpp"
            exe = pathlib.Path(directory) / "test"
            cpp.write_text(source)
            subprocess.run(["g++", "-std=c++17", str(cpp), "-o", str(exe)], check=True)
            subprocess.run([str(exe)], check=True)
