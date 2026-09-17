#pragma once
#include <cstdint>
#include <string>
namespace esphome::nabla_ha {
template<typename Read,typename Complete,typename Clock>
bool read_response(std::string &out,Read read,Complete complete,Clock clock){
  out.clear();char buffer[512];const uint32_t started=clock();
  // Transport completion does not imply its buffered body has been consumed.
  while(true){
    int n=read(buffer,sizeof(buffer));
    if(n<0||clock()-started>5000)return false;
    if(n==0)return complete();
    if(n>int(sizeof(buffer))||out.size()+n>8192)return false;
    out.append(buffer,n);
  }
}
}
