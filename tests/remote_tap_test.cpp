// Native regression for input admission, expiry and automatic release.
#include "../external_components/nabla_display_mirror/remote_tap.h"
#include <initializer_list>
#include <cassert>
using namespace esphome::nabla_display_mirror;
int main(){
  RemoteTap tap;int value;
  assert(parse_coordinate("239",240,value)&&value==239);
  for(auto text:{"240","-1","1.2","","1x","999999999999"})assert(!parse_coordinate(text,240,value));
  assert(tap.enqueue(4,7,10));assert(!tap.enqueue(9,9,11));
  assert(tap.read(20));assert(tap.x==4&&tap.y==7);
  assert(tap.read(99));assert(!tap.read(100));
  assert(tap.enqueue(1,2,200));assert(!tap.read(701));
  assert(tap.enqueue(2,3,800));tap.cancel();assert(!tap.read(800));
  assert(tap.enqueue(0,0,0xfffffff0));assert(tap.read(0xfffffff0));assert(!tap.read(0x40));
}
