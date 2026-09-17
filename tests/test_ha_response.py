from pathlib import Path
import subprocess,tempfile,unittest
ROOT=Path(__file__).resolve().parents[1]
class ResponseTests(unittest.TestCase):
 def test_buffered_body_and_limits(self):
  source=r"""
#include "external_components/nabla_ha/response_reader.h"
#include <cassert>
#include <cstring>
using esphome::nabla_ha::read_response;
int main(){
 std::string out;int calls=0;
 auto buffered=[&](char*b,size_t){if(calls++==0){std::memcpy(b,"{}",2);return 2;}return 0;};
 assert(read_response(out,buffered,[]{return true;},[]{return 1u;}));
 assert(out=="{}"&&calls==2); // completed transport can still have unread bytes
 assert(!read_response(out,[](char*,size_t){return 0;},[]{return false;},[]{return 1u;}));
 assert(!read_response(out,[](char*,size_t){return -1;},[]{return true;},[]{return 1u;}));
 auto flood=[](char*b,size_t n){std::memset(b,'x',n);return int(n);};
 assert(!read_response(out,flood,[]{return false;},[]{return 1u;}));
 assert(out.size()==8192);
 uint32_t now=0;
 assert(!read_response(out,flood,[]{return false;},[&](){now+=6000;return now;}));
}
"""
  with tempfile.TemporaryDirectory() as d:
   path=Path(d);(path/"test.cpp").write_text(source)
   subprocess.run(["g++","-std=c++17","-I",str(ROOT),str(path/"test.cpp"),"-o",str(path/"test")],check=True)
   subprocess.run([str(path/"test")],check=True)
