// Bounded HID report descriptor recognition for byte-aligned keyboard arrays.
#pragma once
#include <cstddef>
#include <cstdint>
namespace esphome::nabla_bt_keyboard {
struct ReportLayout { bool valid{}; uint8_t id{}, bytes{}, modifiers{}, keys{}, count{}; };
inline ReportLayout keyboard_layout(const uint8_t *data,size_t length) {
  struct State { uint32_t page{},size{},count{},id{}; } s{},stack[4]{};
  unsigned depth=0; uint32_t offsets[16]{};
  ReportLayout layouts[16]{};
  bool modifiers[16]{},arrays[16]{};
  uint32_t minimum=0,maximum=0;
  if(length>2048)return {};
  for(size_t i=0;i<length;){
    uint8_t prefix=data[i++]; if(prefix==0xfe)return {};
    size_t n=prefix&3; if(n==3)n=4;
    if(i+n>length)return {};
    uint32_t value=0;for(size_t j=0;j<n;j++)value|=uint32_t(data[i++])<<(8*j);
    unsigned type=(prefix>>2)&3,tag=prefix>>4;
    if(type==1){
      if(tag==0)s.page=value;
      else if(tag==7)s.size=value;
      else if(tag==8){if(!value || value>15)return {};s.id=value;}
      else if(tag==9)s.count=value;
      else if(tag==10){if(depth==4)return {};stack[depth++]=s;}
      else if(tag==11){if(!depth)return {};s=stack[--depth];}
    }else if(type==2){
      if(tag==1)minimum=value;
      if(tag==2)maximum=value;
    }else if(type==0){
      if(tag==8){
        if(s.size>32 || s.count>256 || offsets[s.id]+s.size*s.count>256)return {};
        auto &l=layouts[s.id];l.id=s.id;
        uint32_t offset=offsets[s.id];
        if(!(value&1) && s.page==7){
          if((value&2) && s.size==1 && s.count==8 && minimum==0xe0 && maximum==0xe7 && offset%8==0){
            l.modifiers=offset/8;modifiers[s.id]=true;
          }else if(!(value&2) && s.size==8 && s.count>=1 && s.count<=6 && offset%8==0){
            l.keys=offset/8;l.count=s.count;arrays[s.id]=true;
          }else return {};
        }
        offsets[s.id]+=s.size*s.count;
      }
      minimum=maximum=0;
    }
  }
  for(unsigned id=0;id<16;id++)if(modifiers[id] && arrays[id] && offsets[id]%8==0){
    auto l=layouts[id];l.bytes=offsets[id]/8;l.valid=l.bytes>0 && l.bytes<=32;return l;
  }
  return {};
}
inline bool decode_report(const ReportLayout &l,const uint8_t *data,size_t length,uint8_t out[8]){
  if(!l.valid)return false;
  if(l.id){if(!length || data[0]!=l.id)return false;++data;--length;}
  if(length!=l.bytes || l.modifiers>=length || size_t(l.keys)+l.count>length)return false;
  for(int i=0;i<8;i++)out[i]=0;
  out[0]=data[l.modifiers];
  for(unsigned i=0;i<l.count;i++)out[i+2]=data[l.keys+i];
  return true;
}
}
