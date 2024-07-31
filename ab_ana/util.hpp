#ifndef util_HPP
#define util_HPP 1 

#include <algorithm>
#include <numeric>
#include <map>
#include <vector>
#include <cmath>
#include <cmath>
#include <algorithm>
#include <type_traits>
#include <boost/timer/timer.hpp>
#include <boost/math/statistics/bivariate_statistics.hpp>

#include "TGraph.h"

namespace util{

template <class _tp, class = typename std::enable_if<std::is_integral<_tp>::value>::type>
void read_int(_tp& data, char*& iter){
  char rt[sizeof(_tp)];
  for (int i=sizeof(_tp)-1; i>=0; --i) rt[i] = *iter++;
  data = *reinterpret_cast<_tp*>(rt);}
void read_int(uint8_t& data, char*& iter);

namespace math{
template <class _iter_t>
std::pair<float, float> mean_and_standard_deviation(_iter_t first, _iter_t last){
  auto count = std::distance(first,last);
  if (count<=1) return std::make_pair(std::nanf(""),std::nanf(""));
  float mean = std::accumulate(first,last,0.)/count;
  auto rt = std::accumulate(std::next(first),last,std::pow(*first-mean,2)
      ,[&](float a, float b){return a+std::pow(b-mean,2);});
  return std::make_pair(mean,std::sqrt(rt/(count-1)));

}
double gaus(double*,double*);

}
namespace config{
static std::map<int,std::string> channels_map= {
  {0, "Y55"},  {1, "X55"},  {2, "Y53"},  {3, "X53"},  {4, "Y57"},  {5, "X57"},  {6, "Y59"},  {7, "X59"},
  {8, "Y47"},  {9, "X47"},  {10, "Y45"}, {11, "X45"}, {12, "Y49"}, {13, "X49"}, {14, "Y51"}, {15, "X51"},
  {16, "Y39"}, {17, "X39"}, {18, "Y37"}, {19, "X37"}, {20, "Y41"}, {21, "X41"}, {22, "Y43"}, {23, "X43"},
  {24, "Y31"}, {25, "X31"}, {26, "Y29"}, {27, "X29"}, {28, "Y33"}, {29, "X33"}, {30, "Y35"}, {31, "X35"},
  {32, "Y21"}, {33, "X21"}, {34, "Y23"}, {35, "X23"}, {36, "Y27"}, {37, "X27"}, {38, "Y25"}, {39, "X25"},
  {40, "Y19"}, {41, "X19"}, {42, "Y09"}, {43, "X09"}, {44, "Y17"}, {45, "X17"}, {46, "Y15"}, {47, "X15"},
  {48, "Y05"}, {49, "X05"}, {50, "Y07"}, {51, "X07"}, {52, "Y13"}, {53, "X13"}, {54, "Y11"}, {55, "X11"},
  {56, "Y01"}, {57, "X01"}, {58, "Y03"}, {59, "X03"}, {60, "A2"},  {61, "A0"},  {62, "A3"},  {63, "A1"},
  {64, "Y56"}, {65, "X56"}, {66, "Y52"}, {67, "X58"}, {68, "Y58"}, {69, "Y54"}, {70, "A4"},  {71, "A6"},
  {72, "A7"},  {73, "Y48"}, {74, "X48"}, {75, "A5"},  {76, "X52"}, {77, "X50"}, {78, "Y50"}, {79, "X54"},
  {80, "Y38"}, {81, "X38"}, {82, "Y40"}, {83, "X44"}, {84, "Y44"}, {85, "Y42"}, {86, "X46"}, {87, "Y46"},
  {88, "Y32"}, {89, "Y34"}, {90, "X34"}, {91, "X32"}, {92, "X40"}, {93, "X36"}, {94, "Y36"}, {95, "X42"},
  {96, "Y26"}, {97, "X26"}, {98, "Y22"}, {99, "X28"}, {100, "Y28"},{101, "Y24"},{102, "X30"},{103, "Y30"},
  {104, "Y20"},{105, "Y16"},{106, "X16"},{107, "X20"},{108, "X22"},{109, "X18"},{110, "Y18"},{111, "X24"},
  {112, "Y06"},{113, "X06"},{114, "X08"},{115, "Y12"},{116, "X12"},{117, "Y10"},{118, "X14"},{119, "Y14"},
  {120, "Y00"},{121, "X02"},{122, "Y02"},{123, "X00"},{124, "Y08"},{125, "X04"},{126, "Y04"},{127, "X10"}
};
}


namespace constant{
constexpr static float const strip_spac_x = 10./60; //cm
constexpr static float const strip_spac_y = 10./60; //cm
constexpr static float const drift_speed = 3.5; //cm/us
constexpr static uint16_t const wave_sampling_interval = 40; // ns/per //TODO 
}

inline float adc_to_charge(float adc){ 
  return adc<105. ? 0.0206*adc : adc<330. ? 0.008*adc+1.421 : 0.0069*adc+1.423; }

template <class _iter_t>
TGraph* to_root_graph(_iter_t first, _iter_t last){
  auto* rt = new TGraph();
  std::size_t index=0;
  for (auto iter = first; iter != last; ++iter) 
    rt->SetPoint(index,index++,*iter);
  return rt; }

template <class _iter_t>
void calculate_track_length(_iter_t first, _iter_t last, float& tl
    ,std::function<float(int)> fx, std::function<float(float)> fy){
  if (std::distance(first,last)<=1) return;
  tl = 0.;
  std::map<int,_iter_t> fl;
  for(auto iter = first; iter != last; ++iter)
    fl.emplace(iter->first,iter);
  for (auto iter = fl.begin(); iter != std::prev(fl.end()); ++iter){
    auto iter_next = std::next(iter);
    tl += std::sqrt(fx(iter_next->second->first-iter->second->first)
        +fy(iter_next->second->second-iter->second->second)); }
}

void compress_adcs(uint16_t* src,size_t count,void* dst);
void uncompress_adcs(void* from,uint16_t*);


}
#endif
