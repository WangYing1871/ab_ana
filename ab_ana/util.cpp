#include <bitset>
#include "util.hpp"

namespace util{

void read_int(uint8_t& data, char*& iter){ data = *iter++; }
/* template <size_t > */
void compress_adcs(uint16_t* src, size_t count, void* dst){
  //TODO
  //auto const& t2t = [&](){
  //  uint32_t buf(0);
  //  buf |= (uint32_t)src[0] << 24;
  //  buf |= (uint32_t)src[1] << 16;

  //  
  //};

}

void uncompress_adcs(void* from,uint16_t* to){
}
}


