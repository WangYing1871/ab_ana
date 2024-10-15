#ifndef util_HPP
#define util_HPP 1

#include <algorithm>
#include <optional>
#include <tuple>
#include <string>
#include <unordered_map>
#include <fstream>
#include <filesystem>
namespace util{

template <class _iter>
_iter max_element(_iter first, _iter last){
  if (std::distance(first,last)==0) return first;
  _iter ret=first;
  auto me = *first;
  for (;first != last; ++first) if(*first>me) {me = *first; ret=first;}
  return ret;
}
template <class _iter>
_iter min_element(_iter first, _iter last){
  if (std::distance(first,last)==0) return first;
  _iter ret=first;
  auto me = *first;
  for (;first != last; ++first) if(*first<me) {me = *first; ret=first;}
  return ret;
}


inline void trim_space(std::string& s){
  for (auto iter = s.begin(); iter != s.end(); ++iter)
    if (*iter != ' ' && *iter != '\t')
      for(auto riter = s.rbegin(); riter != s.rend(); ++riter)
        if (*riter!= ' ' && *riter!='\t'){
          s = s.substr(
              std::distance(s.begin(),iter)
              ,std::distance(riter,s.rend())-std::distance(s.begin(),iter)
              ); return; }
  s.clear();
}
namespace _meta{
template <int _idx_v, class... _args>
auto get_v(_args&&... params){
  auto tuple = std::make_tuple<_args...>(std::forward<_args>(params)...);
  if constexpr (_idx_v>=sizeof...(_args)) return std::nullopt;
  else return std::make_optional(std::get<_idx_v>(tuple));
}
}
}

namespace util{

std::unordered_map<std::string,std::string> read_argv(std::string const& pg){
  std::ifstream fin("argv.ini");
  if (!fin){
    throw std::invalid_argument("No InI file");
    exit(0);
  }
  std::string sbuf;
  std::unordered_map<std::string,std::string> ret;
  std::stringstream pgname(""); pgname<<"["<<pg<<"]";
  while(!fin.eof()){
    std::getline(fin,sbuf);
    if (sbuf==pgname.str()){
      while(!fin.eof()){
        auto back = fin.tellg();
        std::getline(fin,sbuf);
        if (sbuf.find_first_of('[') != std::string::npos) { fin.seekg(back); break;}
        if (!sbuf.empty() && sbuf.find_first_of('=') != std::string::npos){
          std::string key = sbuf.substr(0,sbuf.find_first_of('='));
          std::string value =  sbuf.substr(sbuf.find_first_of('=')+1);
          trim_space(key); trim_space(value);
          if (!key.empty() && !value.empty()) ret[key]=value;
        }
      }
    }
  }
  return ret;
}

template <class _tp
  ,int _bytes_v = sizeof(_tp)
  ,bool _is_big_end_v = false
  ,class = typename std::enable_if<std::is_integral<_tp>::value>::type>
inline void read_int(_tp& data, char*& iter){
  static_assert(_bytes_v<=sizeof(_tp));
  char rt[_bytes_v];
  if constexpr (_is_big_end_v) for (int i=0; i<_bytes_v; ++i) rt[i] = *iter++;
  else for (int i=_bytes_v-1; i>=0; --i) rt[i] = *iter++;
  data = *reinterpret_cast<_tp*>(rt);
}
inline void read_int(uint8_t& data, char*& iter){ data = *iter++; }

enum class e_command_address : uint16_t{
  k_FEID = 0x1806
  ,k_ChannelID = 0x1808
  ,k_Threshold_Tag= 0x180B
  ,k_Threshold_Value = 0x180A
  ,k_Config = 0x180C
};
template <class... _args>
void generate_frame(e_command_address command, char* positon
    , _args&&... params){
  auto const& head_write = [&](e_command_address cmd)->void{
    uint16_t value = uint16_t(cmd);
    positon[0] = 0x00 + (value&0x0F);
    positon[1] = 0x10 + ((value>>4)&0x0F);
    positon[2] = 0x20 + ((value>>8)&0x0F);
    positon[3] = 0x30 + ((value>>12)&0x0F); };
  if (!positon) return;
  switch(command){
    case (e_command_address::k_FEID):{
      head_write(command);
      auto opt_fec_id = _meta::get_v<0>(std::forward<_args>(params)...);
      uint8_t fec_id;
      if constexpr(std::is_same<decltype(opt_fec_id),std::nullopt_t>::value)
        fec_id = 0;
      else fec_id = opt_fec_id.value();
      positon[4] = 0x40+fec_id%16;
      positon[5] = 0x50+fec_id/16;
      positon[6] = 0x60;
      positon[7] = 0x70;
      positon[8] = 0x83;
      break;
    }
    case (e_command_address::k_ChannelID):{
      head_write(command);
      auto opt_cid = _meta::get_v<0>(std::forward<_args>(params)...);
      uint8_t cid;
      if constexpr(std::is_same<decltype(opt_cid),std::nullopt_t>::value)
        cid = 0;
      else cid = opt_cid.value();
      positon[4] = 0x40+cid%16;
      positon[5] = 0x50+cid/16;
      positon[6] = 0x60;
      positon[7] = 0x70;
      positon[8] = 0x83;
      break;
    }
    case (e_command_address::k_Threshold_Tag):{
      head_write(command);
      positon[4] = 0x40;
      positon[5] = 0x50;
      positon[6] = 0x60;
      positon[7] = 0x70;
      positon[8] = 0x83;
      break;
    }
    case (e_command_address::k_Threshold_Value):{
      head_write(command);
      auto mean_opt = _meta::get_v<0>(std::forward<_args>(params)...);
      uint32_t mean;
      if constexpr(std::is_same<decltype(mean_opt),std::nullopt_t>::value)
        mean = 0;
      else mean = mean_opt.value();
      auto sigma_opt = _meta::get_v<1>(std::forward<_args>(params)...);
      uint32_t sigma;
      if constexpr(std::is_same<decltype(sigma_opt),std::nullopt_t>::value)
        mean = 0;
      else sigma = sigma_opt.value();
      auto compres_opt = _meta::get_v<2>(std::forward<_args>(params)...);
      uint32_t compres;
      if constexpr(std::is_same<decltype(compres_opt),std::nullopt_t>::value)
        compres = 0;
      else compres = compres_opt.value();
      uint64_t value = mean+compres*sigma;
      uint16_t value_u16 = value;
      if (value>0xFFFF){
        std::cerr<<"Warning!! threshold set overflw to uint16_t max! Set to '0xFFFF'";
        value_u16 = 0xFFFF; }
      positon[4] = 0x40+(value_u16&0x0F);
      positon[5] = 0x50+((value_u16>>4)&0x0F);
      positon[6] = 0x60+((value_u16>>8)&0x0F);
      positon[7] = 0x70+((value_u16>>12)&0x0F);
      positon[8] = 0x83;
      break;
    }
    case (e_command_address::k_Config):{
      head_write(command);
      positon[4] = 0x41;
      positon[5] = 0x50;
      positon[6] = 0x60;
      positon[7] = 0x70;
      positon[8] = 0x83;
      break;
    }
    default:
      break;
  }
}
}

#endif
