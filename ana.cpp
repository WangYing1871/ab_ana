//--------------------------------Stamp-------------------------------
//^-^ Author: Zhi Heng            Email: 2835516101@qq.com
//^-^ Time: 2024-07-18 10:02:05   Posi: Hefei
//^-^ File: unpack.cpp
//--------------------------------------------------------------------
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <mutex>
#include <numeric>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include <boost/timer/timer.hpp>
#include <boost/math/statistics/bivariate_statistics.hpp>

#include "TFile.h"
#include "TGraph.h"
#include "TFolder.h"
#include "TTree.h"
#include "TPoint.h"
#include "TSpectrum.h"
#include "TH1F.h"
#include "TH1I.h"
#include "TH2F.h"
#include "TF1.h"
#include "TCanvas.h"

#define info_out(X) std::cout<<"==> "<<__LINE__<<" "<<#X<<" |"<<(X)<<"|\n"
namespace util{
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

//template <class _itera_t, class _iterb_t>
//float correlation_coefficient




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
        +fy(iter_next->second->second-iter->second->second));
  }

}
//
//std::vector<Point> findNearestNeighbors(const std::vector<Point>& points, float& tracklength) {
//    std::vector<Point> result;
//    std::set<Point> visited;
//    
//    // find starting point, with max Z
//    Point startingPoint = *std::max_element(points.begin(), points.end());
//
//    // std::cout << "Start position: (" << startingPoint.x << ", " << startingPoint.y << ")" << std::endl;
//
//    //define maximum loop time in order to avoid the case that the visited points is always smaller than the total points (maybe due to 2 points overlap)
//    int n_loop = 0;
//    int maximum = points.size();
//
//    while (visited.size() < points.size()) {
//        result.push_back(startingPoint);
//        visited.insert(startingPoint);
//
//        // find nearest remaining point to this point
//        float minDistance = std::numeric_limits<float>::max();
//        Point nextPoint;
//
//        for (const Point& neighbor : points) {
//            if (visited.find(neighbor) == visited.end()) {
//                float distance = calculateDistance(startingPoint, neighbor);
//                if (distance < minDistance) {
//                    minDistance = distance;
//                    nextPoint = neighbor;
//                }
//            }
//        }
//
//        startingPoint = nextPoint;
//
//        n_loop++;
//        if(n_loop>=maximum || (startingPoint.x==0 && startingPoint.y==0)) break;
//        tracklength += minDistance;
//    }
//
//    return result;
//}
}

template <class _tp, class = typename std::enable_if<std::is_integral<_tp>::value>::type>
void read_int(_tp& data, char*& iter){
  char rt[sizeof(_tp)];
  for (int i=sizeof(_tp)-1; i>=0; --i) rt[i] = *iter++;
  data = *reinterpret_cast<_tp*>(rt);}

void read_int(uint8_t& data, char*& iter){ data = *iter++; }

struct baseline{
  typedef baseline self_t;
  static constexpr uint16_t const s_head_tag = 0x55aa;
  static constexpr uint16_t const s_tail_tag = 0x5aa5;

  bool head_valid() const {return head_tag==s_head_tag;}
  bool tail_valid() const {return tail_tag==s_tail_tag;}

  uint16_t head_tag = 0;
  uint16_t package_size;
  uint8_t type;
  uint32_t evt_id;
  uint8_t trigger_idx;
  //uint8_t channel_idx;
  uint16_t channel_idx;
  uint16_t adc[1024];
  uint32_t crc32;
  uint16_t tail_tag;

  void clear(){ for(auto&& x : adc) x = 0; }
  bool parse(char*& iter){
    read_int(this->head_tag,iter);
    if (!this->head_valid()) return false;
    auto data_restore = *this; this->clear(); auto iter_restore = iter;
    read_int(this->package_size,iter);
    read_int(this->type,iter);
    read_int(this->evt_id,iter);
    read_int(this->trigger_idx,iter);
    read_int(this->channel_idx,iter);
    for (auto&& x : this->adc) read_int(x,iter);
    read_int(this->crc32,iter);
    read_int(this->tail_tag,iter);
    if (!this->tail_valid()) {*this = data_restore; iter=iter_restore; return false;}
    return true; };

  std::ostream& display(std::ostream& os = std::cout){
    return os<<std::hex<<"==================================================\n"
      <<"  head_tag\t"<<(int)head_tag<<"\n"
      <<"  package_size\t"<<(int)package_size<<"\n"
      <<"  type\t"<<(int)type<<"\n"
      <<"  evt_id\t"<<(int)evt_id<<"\n"
      <<"  trigger_idx\t"<<(int)trigger_idx<<"\n"
      <<"  channel_idx\t"<<(int)channel_idx<<"\n"
      <<"  crc32\t"<<(int)crc32<<"\n"
      <<"  tail_tag\t"<<(int)tail_tag<<"\n"
      <<std::dec
      ;

  }

  size_t get_channel_id() const{ return channel_idx&0x00FF; }
  size_t get_event_id() const {return (evt_id<<8)+trigger_idx;}
};
struct signal{
  typedef signal self_t;
  static constexpr uint16_t const s_head_tag = 0x55aa;
  static constexpr uint16_t const s_tail_tag = 0x5aa5;
  inline bool head_valid() const {return head_tag==s_head_tag;}
  inline bool tail_valid() const {return tail_tag==s_tail_tag;}

  uint16_t head_tag = 0;
  uint16_t package_size;
  uint8_t type;
  uint32_t evt_id;
  uint8_t trigger_idx;
  uint16_t channel_idx;
  uint16_t amp;
  uint16_t half_time;
  uint16_t peak_time;
  uint32_t crc32;
  uint16_t tail_tag;

  bool valid() const{
    return package_size==sizeof(*this) && head_valid() && tail_valid(); }

  std::ostream& display(std::ostream& os = std::cout){
    return os<<std::hex<<"==================================================\n"
      <<"  head_tag\t"<<(int)head_tag<<"\n"
      <<"  package_size\t"<<(int)package_size<<"\n"
      <<"  type\t"<<(int)type<<"\n"
      <<"  evt_id\t"<<(int)evt_id<<"\n"
      <<"  trigger_idx\t"<<(int)trigger_idx<<"\n"
      <<"  channel_idx\t"<<(int)channel_idx<<"\n"
      <<"  amp\t"<<(int)amp<<"\n"
      <<"  half_time\t"<<(int)half_time<<"\n"
      <<"  peak_time\t"<<(int)peak_time<<"\n"
      <<"  crc32\t"<<(int)crc32<<"\n"
      <<"  tail_tag\t"<<(int)tail_tag<<"\n"
      <<std::dec
      ;

  }

  bool parse(char*& iter,char* const& iter_end){
    if ((size_t)std::distance(iter,iter_end)<sizeof(*this)) return false;
    read_int(this->head_tag,iter);
    if (!this->head_valid()) return false;
    auto data_restore = *this;auto iter_restore = iter;
    read_int(this->package_size,iter);
    read_int(this->type,iter);
    read_int(this->evt_id,iter);
    read_int(this->trigger_idx,iter);
    read_int(this->channel_idx,iter);
    read_int(this->amp,iter);
    read_int(this->half_time,iter);
    read_int(this->peak_time,iter);
    read_int(this->crc32,iter);
    read_int(this->tail_tag,iter);
    if (!this->tail_valid()) {*this = data_restore; iter=iter_restore; return false;}
    return true;
  };
  size_t get_channel_id() const{ return channel_idx&0x00FF; }
  size_t get_event_id() const {return evt_id;}
};

template <class _tp>
struct data_pool{
  typedef _tp data_frame_t;
  std::mutex m_mutex;
  char data[1024*1024];
  char* m_current,* m_end;

public:
  data_pool() {m_current = m_end = data;}
  ~data_pool() noexcept = default;
  

  std::size_t size() const {return std::size_t(std::distance(m_current,m_end));}
  void reset() {/*std::lock_guard lock(m_mutex);*/std::copy(m_current,m_end,data);}

  bool is_not_full(){ return std::distance(m_end,data+1024*1024)>1024;}
  bool is_not_empty(){ return size()>=sizeof(data_frame_t); }

  std::condition_variable _is_cv_not_full;
  std::condition_variable _is_cv_not_empty;
};

struct painter{
  std::string m_name;

};

template <class _tp, class _up>
struct signal_handler{
  typedef _tp baseline_pack_t;
  typedef _up signal_pack_t;
  std::string m_file_name;
  std::string m_baseline_name;
  bool _is_baseline_viewer = false;
  bool _is_signal_viewer = false;
  bool _is_draw_baseline = false;
  bool _is_draw_signal = false;

  void baseline_viewer(bool v) {_is_baseline_viewer = v;}
  void signal_viewer(bool v) {_is_signal_viewer = v;}
  void draw_baseline(bool v) {_is_draw_baseline = v;}
  void draw_signal(bool v) {_is_draw_signal = v;}

  struct mean_sigma{
    float mean;
    float sigma;
    void set(std::pair<float,float> a) {mean = a.first, sigma = a.second;}
    mean_sigma() = default;
    mean_sigma(float a, float b):mean(a),sigma(b) {}
    ~mean_sigma() = default;
  };
  std::unordered_map<size_t,mean_sigma> m_baseline_table;
  std::unordered_map<size_t,std::unordered_map<uint8_t,std::array<uint16_t,1024>>> m_signal_table;
  void file_name(std::string const& v) {m_file_name = v;}
  void baseline_name(std::string const& v) {m_baseline_name = v;}

  void draw(std::vector<TH1I*> const& v
      ,std::string const& px_name
      ,std::string const& py_name
      ,std::string const& pz_name){
    auto* canvas_x = new TCanvas("canvas_x","canvas_x",1200,900);
    auto* canvas_y = new TCanvas("canvas_y","canvas_y",1200,900);
    auto* canvas_anti = new TCanvas("canvas_anti","canvas_anti",1200,900);
    std::map<int,TH1I*> map01, map02, map03;
    for (auto&& x : v){
      if (x->GetName()[0]=='X') map01.emplace(std::atoi(x->GetName()+1)+1,x);
      else if(x->GetName()[0]=='Y') map02.emplace(std::atoi(x->GetName()+1)+1,x);
      else map03.emplace(std::atoi(x->GetName()+1)+1,x);
    }
    canvas_x->Clear(); canvas_x->Divide(6,10);
    for (auto&& [x,y] : map01) canvas_x->cd(x), y->Draw();
    canvas_y->Clear(); canvas_y->Divide(6,10);
    for (auto&& [x,y] : map02) canvas_y->cd(x), y->Draw();
    canvas_anti->Clear(); canvas_anti->Divide(2,4);
    for (auto&& [x,y] : map03) canvas_anti->cd(x), y->Draw();
    canvas_x->SaveAs(px_name.c_str());
    canvas_y->SaveAs(py_name.c_str());
    canvas_anti->SaveAs(pz_name.c_str());
  }

  void get_baseline(){
    m_baseline_table.clear();
    std::ifstream in_stream(m_baseline_name,std::ios_base::binary);
    in_stream.seekg(0,std::ios_base::end);
    size_t fsz = in_stream.tellg();
    char* file_buf = new char[fsz];
    in_stream.seekg(0,std::ios_base::beg);
    in_stream.read(file_buf,fsz);
    baseline_pack_t parser;
    std::unordered_map<size_t, std::vector<std::array<uint16_t,1024>>> channel_adcs;
    for (auto iter = file_buf; (size_t)std::distance(file_buf,iter)<fsz; ){
      if (parser.parse(iter)){
        std::array<uint16_t,1024> adcs;
        std::transform(std::begin(parser.adc),std::end(parser.adc),std::begin(adcs)
            ,[](uint16_t a){return (uint16_t)(a&0x0FFF);});
        channel_adcs[parser.get_channel_id()].emplace_back(adcs);
      }
    }
    
    //boost::timer::auto_cpu_timer progress;


    std::vector<TH1I*> a00;
    
    for (auto&& [x,y] : channel_adcs){
      std::vector<uint16_t> buf(y.size()*1024);
      for (size_t i=0; auto&& y0 : y) std::copy(y0.begin(),y0.end(),buf.begin()+1024*i++);
      auto ms = util::math::mean_and_standard_deviation(buf.begin(),buf.end());
      m_baseline_table[x].set(ms);


      std::stringstream sstr(""); sstr<<util::config::channels_map[x];
      auto* f1 = new TH1I(sstr.str().c_str(),sstr.str().c_str()
          ,(int)3*ms.second,ms.first-3*ms.second,ms.first+3*ms.second);
      for(auto&& x : buf) f1->Fill(x);
      a00.emplace_back(f1);

    }
    in_stream.close();
    delete file_buf;
    if (_is_baseline_viewer){
      auto* fout = new TFile("baseline.root","recreate");
      fout->cd();
      for (size_t i=0; auto&& [x,y] : channel_adcs){
        std::stringstream sstr; sstr<<i<<"_"<<x;
        auto* folder = new TFolder(sstr.str().c_str(),sstr.str().c_str());
        for(std::size_t j=0; auto&& y0 : y){
          auto* gr = util::to_root_graph(y0.begin(),y0.end());
          std::stringstream sstr1;
          sstr1.clear(); sstr1<<i<<"_"<<j++;
          gr->SetName(sstr1.str().c_str());
          gr->SetTitle(sstr1.str().c_str());
          folder->Add(gr);
        }
        ++i;
        folder->Write();
      }
      auto* h1_mean = new TH1F("mean","mean",500,500,1000);
      auto* h1_sigma = new TH1F("sigma","sigma",500,0,100);
      for (auto&& [x,y] : m_baseline_table) h1_mean->Fill(y.mean), h1_sigma->Fill(y.sigma);

      std::string txt_name = m_baseline_name.substr(0,m_baseline_name.find_last_of("."))+".txt";
      std::ofstream of_text(txt_name.c_str());
      std::time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
      of_text<<"#baseline txt of "
        <<std::put_time(std::localtime(&t), "%c %Z")<<"\n";
      //of_text<<"#adc_name\tmean\tsigma\tChi/NDF\n";
      of_text<<"#adc_name\tmean\tsigma\n";
      for (auto&& x :a00){
        auto* f1 = new TF1("f1","gaus",x->GetMean()-3*x->GetRMS(), x->GetMean()+3*x->GetRMS());
        x->Fit(f1,"RQ");
        of_text<<x->GetName()<<"\t"<<f1->GetParameter(1)<<"\t"<<f1->GetParameter(2)<<"\n"; }
      if (_is_draw_baseline) draw(a00,"baseline_X.pdf","baseline_Y.pdf","baseline_anti.pdf");
      h1_mean->Write(); h1_sigma->Write();
      fout->Write(); fout->Close();
      of_text.close();
    }else{
      for (auto&& x : a00) delete x;
    } 


  }

  //FIXME TODO !!! can't use now
  /*
  void get_signal(){
    data_pool<signal_pack_t> mem_pool;
    info_out("get_signal ");

    std::ifstream fin(m_file_name.c_str(),std::ios_base::binary);
    auto const& push = [&](){
      std::unique_lock lock(mem_pool.m_mutex);
      mem_pool._is_cv_not_full.wait(lock
          ,std::bind(&data_pool<signal_pack_t>::is_not_full,std::addressof(mem_pool)));
      //bool wait_status = mem_pool._is_cv_not_full.wait_until(lock,std::chrono::steady_clock::now() + std::chrono::seconds(5)
      //    ,std::bind(&data_pool::is_not_full,std::addressof(mem_pool)));
      //if (!wait_status){
      //}
      fin.read(mem_pool.m_end,1024);
      mem_pool.m_end += fin.gcount();
      //info_out(mem_pool.size());
      lock.unlock();
      //mem_pool._is_cv_not_empty.notify_all();
      if (mem_pool.is_not_empty()) mem_pool._is_cv_not_empty.notify_one();
    };
    auto const& pop = [&](){
      std::unique_lock lock(mem_pool.m_mutex);
      char*& iter = mem_pool.m_current;
      mem_pool._is_cv_not_empty.wait(lock
          ,std::bind(&data_pool<signal_pack_t>::is_not_empty,std::addressof(mem_pool)));
      signal_pack_t parser;
      do{
        bool status = parser.parse(mem_pool.m_current);
        if (status){
          info_out("package get");
        }
        info_out(mem_pool.size());
      }while(mem_pool.is_not_empty());

      //while(true){
      //  info_out("client");
      //  if (parser.parse(iter,mem_pool.m_end)){
      //    info_out("~~~~~ client ====> A package get");

      //  }else if(mem_pool.size()>=sizeof(signal)){
      //    continue;
      //  }else{
      //    mem_pool.reset();
      //    break;
      //  }
      //}
      lock.unlock();
      if (mem_pool.is_not_full()) mem_pool._is_cv_not_full.notify_one();
    };

    std::thread bytes_in([&](){
        while(!fin.eof()){
          push();
        }

        });
    std::thread bytes_out([&](){
        while(!fin.eof() || mem_pool.is_not_empty()){
          pop();
        }
        });
    bytes_in.join();
    //std::this_thread::sleep_for(std::chrono::seconds(10));
    bytes_out.join();


  }
  */
  void get_signal(){
    if (m_file_name=="") return;
    std::ifstream in_stream(m_file_name.c_str(),std::ios_base::binary);
    in_stream.seekg(0,std::ios_base::end);
    size_t fsz = in_stream.tellg();
    char* file_buf = new char[fsz];
    in_stream.seekg(0,std::ios_base::beg);
    in_stream.read(file_buf,fsz);
    signal_pack_t parser;

    std::map<size_t,TH1I*> a00;
    auto* sum_anti = new TH1I("sum_anti","sum_anti",4096,0,4096);
    if (_is_signal_viewer){
      for(auto&& [x,y] : util::config::channels_map){
        std::stringstream sstr; sstr<<y<<"_S";
        a00.emplace(x,new TH1I(sstr.str().c_str(),
              sstr.str().c_str(),4096,0,4096));
      }
    }
    

    for (auto iter = file_buf; (size_t)std::distance(file_buf,iter)<fsz; ){
      if (parser.parse(iter)){
        auto& ref = m_signal_table[parser.get_event_id()][parser.get_channel_id()];
        std::transform(std::begin(parser.adc),std::end(parser.adc),std::begin(ref)
            ,[](uint16_t a){return (uint16_t)(a&0x0FFF);});
        if (_is_signal_viewer){ 
          auto max = *std::max_element(std::begin(ref),std::end(ref));
          a00[parser.get_channel_id()]->Fill(max);
          if (util::config::channels_map[parser.get_channel_id()][0]=='A') 
            sum_anti->Fill(max);
        }
      }
    }

    delete[] file_buf;
    in_stream.close();
    if (_is_signal_viewer){
      auto* file = new TFile("signal.root","recreate");
      file->cd();
      for (auto&& [x,y] : a00 ) y->Write();
      sum_anti->Write();
      file->Write(); file->Close();
    }
    if (_is_draw_signal){
      std::vector<TH1I*> buf; for(auto&& x : a00) buf.emplace_back(x.second);
      draw(buf,"signal_X.pdf","signal_Y.pdf","signal_anti.pdf");
      TCanvas* c1 = new TCanvas("c1","c1",800,600);
      c1->cd(); sum_anti->Rebin(8); sum_anti->Draw();
      c1->SaveAs("Anti.pdf");
    }
  }



  void get_status(){
    std::ifstream fin(m_file_name.c_str(),std::ios_base::binary);
    for (int i=0; i<100; ++i){
      char* data_buf = new char[128];

      //auto bits = fin.read(data_buf,128);
    }

    //read(10000);
    //if (parse(iter) )
  }


};
//---------------------------------------------------------------------
struct adc_data_t{

  std::vector<int>* fec_ptr;
  size_t event_id;
  uint32_t n_hits;
  std::vector<int> fec;
  std::vector<int> chip;
  std::vector<int> chn;

  std::vector<uint8_t> channels;
  std::vector<uint16_t> adcs;

  std::vector<uint32_t> sum_adc;
  std::vector<uint16_t> max_adc;
  std::vector<float> max_point;
  uint64_t summax_adc;
  std::vector<int> pixel_x, pixel_y;


  std::array<uint16_t,1024> get_wave(size_t idx) const{
    if (idx>=n_hits) throw std::invalid_argument("rnage error");
    auto iter = adcs.begin()+1024*idx;
    std::array<uint16_t,1024> rt;
    std::copy(iter,iter+1024,rt.begin());
    return rt; }

  void clear(){
    event_id = std::numeric_limits<size_t>::max();
    n_hits = 0;
    fec.clear(); chip.clear(); chn.clear();
    channels.clear(); adcs.clear();
    sum_adc.clear(); max_adc.clear(); max_point.clear();
    summax_adc = 0;
    pixel_x.clear(); pixel_y.clear();
  }

  void link_in_tree(TTree* tree){
    tree->Branch("nHits",&n_hits);
    tree->Branch("Fec",&fec);
    tree->Branch("Chip",&chip);
    tree->Branch("Chn",&chn);
    tree->Branch("Channel",&channels);
    tree->Branch("ADC",&adcs);
    tree->Branch("sumADC",&sum_adc);
    tree->Branch("maxADC",&max_adc);
    tree->Branch("maxPoint",&max_point);
    tree->Branch("summaxADC",&summax_adc);
    tree->Branch("pixelX",&pixel_x);
    tree->Branch("pixelY",&pixel_y);
  }
  void link_out_tree(TTree* tree){
    tree->SetBranchAddress("nHits",&n_hits);
   // std::vector<int>* fec_ptr = &fec; 
    tree->SetBranchAddress("Fec",&fec_ptr);
    //auto* chip_ptr = &chip; tree->SetBranchAddress("Chip",&chip_ptr);
    //auto* chn_ptr = &chn; tree->SetBranchAddress("Chn",&chn_ptr);
    //auto* channels_ptr = &channels; tree->SetBranchAddress("Channel",&channels_ptr);
    //auto* adcs_ptr = &adcs; tree->SetBranchAddress("ADC",&adcs_ptr);
    //auto* sum_adc_ptr = &sum_adc; tree->SetBranchAddress("sumADC",&sum_adc_ptr);
    //auto* max_adc_ptr = &max_adc; tree->SetBranchAddress("maxADC",&max_adc_ptr);
    //auto* max_point_ptr = &max_point; tree->SetBranchAddress("maxPoint",&max_point_ptr);
    //tree->SetBranchAddress("summaxADC",&summax_adc);
    //auto* pixel_x_ptr = &pixel_x; tree->SetBranchAddress("pixelX",&pixel_x_ptr);
    //auto* pixel_y_ptr = &pixel_y; tree->SetBranchAddress("pixelY",&pixel_y_ptr);
  }




  void fill(TTree* tree){
    tree->Fill();
  }
  std::ostream& display(std::ostream& os = std::cout){
    return os<<"n_hits\t"<<n_hits<<"\n"
      <<"channels\t"<<channels.size()<<"\n";

  }
};
//---------------------------------------------------------------------
struct rec_data_store{  //TODO rename
  float m_energy;
  float m_hits;
  uint16_t m_hits_x, m_hits_y;
  float m_ratio;
  float m_tracklength_x, m_tracklength_y;
  int m_start_x, m_start_y;
  float m_incident_angle;
  float m_rho_x, m_rho_y;
  float m_time_diff;
  uint8_t m_reco_finish;

  void reset(){
    m_energy = 0.;
    m_hits = 0.;
    m_hits_x = m_hits_y = 0;
    m_ratio = 0.;
    m_tracklength_x = m_tracklength_y = 0.;
    m_start_x = m_start_y = 0;
    m_incident_angle = 0.;
    m_rho_x = m_rho_y = 0.;
    m_reco_finish = 0;
    m_time_diff = 0.;
  }
  
  
  
};
//template <class _tp>
//struct rec_data : public _tp{ //hook TODO
struct rec_data : public rec_data_store{
  typedef rec_data self_t;
  typedef rec_data_store base_t;
  //typedef _tp base_t;
  //typedef _tp payload_t;
  typedef struct{
    typedef bool result_type;
    typedef std::pair<int,float> element_t;
    result_type operator()(element_t const& a, element_t const& b) const{
      return a.second<b.second; }
  } points_sort_t;

  bool _is_multi_peak = false;
  TSpectrum* m_spec;
  signal_handler<baseline,baseline>* eledata_handlder;


  void reco(adc_data_t& raw){
    reset();
    float edep = 0.;
    uint16_t hits = 0;
    uint16_t anti_hits = 0;
    float amp_sum = 0;

    std::multiset<std::pair<int,float>,points_sort_t> points_x;
    std::multiset<std::pair<int,float>,points_sort_t> points_y;
    std::vector<std::pair<uint16_t,float>> max_adc_x;
    std::vector<std::pair<uint16_t,float>> max_adc_y;
    for(size_t i=0; i<raw.n_hits; ++i){
      m_energy += raw.max_adc[i];
      auto wave = raw.get_wave(i);
      //auto iter_max = std::begin(wave)+(size_t)raw.max_point[i];
      auto iter_max = std::max_element(std::begin(wave),std::end(wave));
      uint8_t ch_id = raw.channels[i];
      float c_mean = eledata_handlder->m_baseline_table[ch_id].mean;
      float c_sigma = eledata_handlder->m_baseline_table[ch_id].sigma;

      //if (*iter_max <= 5*c_sigma) continue;
      //edep += util::adc_to_charge(*iter_max-c_mean);
      edep = *iter_max;
      

      //amp_sum += *std::max_element(std::begin(wave),std::end(wave))-c_mean; //TODO
      for (auto iter = iter_max; iter != iter_max-100; --iter){
        float thr = c_mean+0.5*(*iter_max-c_mean);
        if (*iter<=thr){
          raw.max_point[i] = std::distance(std::begin(wave),iter) + (thr-*iter)/(*(iter+1)-*iter);
          break;
        }
      }

      if (raw.pixel_x[i] != 0 && raw.pixel_y[i]==0){
        if (!_is_multi_peak){
          m_hits_x++; hits++;
          points_x.emplace(raw.pixel_x[i],raw.max_point[i]);
          max_adc_x.emplace_back(std::make_pair(raw.max_adc[i]-c_mean,raw.max_point[i]));
        }else{
          //TODO
        }

      }else if(raw.pixel_x[i]==0 && raw.pixel_y[i]!=0){
        if (!_is_multi_peak){
          m_hits_y++; hits++;
          points_y.emplace(raw.pixel_y[i],raw.max_point[i]);
          max_adc_y.emplace_back(std::make_pair(raw.max_adc[i]-c_mean,raw.max_point[i]));
        }else{
          //TODO
        }
      }else if(raw.pixel_x[i]==-1 && raw.pixel_y[i]==-1){
        anti_hits++;
      }else{
      }
    }
    if (points_x.size()==0 && points_y.size()==0) return;
    float time_range[2] = {0.,0.};
    time_range[0] = points_x.size()==0 ? points_y.begin()->second : 
      points_y.size()==0 ? points_x.begin()->second : std::min(points_x.begin()->second,points_y.begin()->second);
    time_range[1] = points_x.size()==0 ? std::prev(points_y.end())->second : 
      points_y.size()==0 ? std::prev(points_x.end())->second :
      std::max(std::prev(points_x.end())->second,std::prev(points_y.end())->second);

    typedef std::pair<uint16_t,float> maxadc_tm_t;
    auto max_adc_point_x = std::max_element(max_adc_x.begin()
        ,max_adc_x.end(),[](maxadc_tm_t const& a, maxadc_tm_t const& b)->bool{ return a.first<b.first;});
    auto max_adc_point_y = std::max_element(max_adc_y.begin()
        ,max_adc_y.end(),[](maxadc_tm_t const& a, maxadc_tm_t const& b)->bool{ return a.first<b.first;});
    uint16_t maxadc_x = max_adc_x.size()==0 ? 0 : max_adc_point_x->first;
    uint16_t maxadc_y = max_adc_y.size()==0 ? 0 : max_adc_point_y->first;

    m_ratio = time_range[0]==time_range[1] ? 0. :
      maxadc_x>maxadc_y ? 
      (time_range[1]-max_adc_point_x->second)/(time_range[1]-time_range[0]):
      (time_range[1]-max_adc_point_y->second)/(time_range[1]-time_range[0]);
    m_time_diff = time_range[1]-time_range[0];

    using util::constant::strip_spac_x;
    using util::constant::strip_spac_y;
    using util::constant::drift_speed;
    using util::constant::wave_sampling_interval;
    auto const& fx = [&](int a)->float{return std::pow(a*strip_spac_x,2);};
    auto const& fy = [&](int a)->float{return std::pow(a*strip_spac_y,2);};
    auto const& fz = [&](float a)->float{return std::pow(a*drift_speed*wave_sampling_interval*1.e-3,2);};
    util::calculate_track_length(points_x.begin(),points_x.end(),m_tracklength_x
        ,fx,fz);
    util::calculate_track_length(points_y.begin(),points_y.end(),m_tracklength_y
        ,fx,fz);

    if (points_x.size()>=1)m_start_x = std::prev(points_x.end())->first;
    if (points_y.size()>=1)m_start_y = std::prev(points_y.end())->first;

    m_hits = std::sqrt(m_hits_x*m_hits_x+m_hits_y*m_hits_y); //TODO
    float tl_xy = std::sqrt(std::pow((m_hits_x*strip_spac_x),2)+std::pow(m_hits_y*strip_spac_y,2));
    float tl_z = (time_range[1]-time_range[0])*wave_sampling_interval*drift_speed*1.e-3;
    if (tl_xy!=0.) m_incident_angle = tl_z/tl_xy;

    std::vector<double> array_buf_x, array_buf_y;
    for (auto&& [x,y] : points_x)
      array_buf_x.emplace_back((double)x), array_buf_y.emplace_back((double)y);
    m_rho_x = array_buf_x.size()>=2 ?boost::math::statistics::correlation_coefficient(array_buf_x,array_buf_y) : 0.;
    array_buf_x.clear(), array_buf_y.clear();
    for (auto&& [x,y] : points_y)
      array_buf_x.emplace_back((double)x), array_buf_y.emplace_back((double)y);
    m_rho_y = array_buf_y.size()>=2 ? boost::math::statistics::correlation_coefficient(array_buf_x,array_buf_y) : 0.;
    m_reco_finish = 1;

    m_h1_spectrum->Fill(edep);
    m_h1_tot_hits->Fill(hits);
    m_h1_x_hits->Fill(m_hits_x);
    m_h1_y_hits->Fill(m_hits_y);
    m_h1_ratio->Fill(m_ratio);
    m_h1_time_diff->Fill(m_time_diff);
    m_h1_drift_z->Fill(tl_z);
    m_h1_drift_xy->Fill(tl_xy);
    m_h1_angle->Fill(90-std::atan(m_incident_angle)*180./3.14159);
    m_h1_sum_max->Fill(amp_sum);


    m_h2_ene_vs_hits->Fill(edep,m_hits);
    m_h2_start_pos->Fill(m_start_x,m_start_y);






  }

  //shouble be manager by common base struct, which has pure virtual method
  //[link_in_tree],[link_out_tree], [run(n_threads)] and so on.
  //
  void link_in_tree(TTree* tree){
  }

  void link_out_tree(TTree* tree){
    if (!tree) return;
    tree->Branch("energy",std::addressof(m_energy));
    tree->Branch("hits",std::addressof(m_hits));
    tree->Branch("hits_x",std::addressof(m_hits_x));
    tree->Branch("hits_y",std::addressof(m_hits_y));
    tree->Branch("ratio",std::addressof(m_ratio));
    tree->Branch("time_diff",std::addressof(m_time_diff));
    tree->Branch("tracklength_x",std::addressof(m_tracklength_x));
    tree->Branch("tracklength_y",std::addressof(m_tracklength_y));
    tree->Branch("start_x",std::addressof(m_start_x));
    tree->Branch("start_y",std::addressof(m_start_y));
    tree->Branch("incident_angle",std::addressof(m_incident_angle));
    tree->Branch("rho_x",std::addressof(m_rho_x));
    tree->Branch("rho_y",std::addressof(m_rho_y));
    tree->Branch("valid",std::addressof(m_reco_finish));
  }

  void init_histo(){
    m_h1_spectrum = new TH1F("spectrum","spectrum",5000,0,1000);
    m_h1_tot_hits = new TH1I("TotalHits","TotalHits",60,0,60);
    m_h1_x_hits = new TH1I("TotalHits_X","TotalHits_X",30,0,30);
    m_h1_y_hits = new TH1I("TotalHits_Y","TotalHits_Y",30,0,30);
    m_h1_ratio = new TH1F("MaxHit_Ratio","MaxHit_Ratio",100,0,1);
    m_h1_time_diff = new TH1F("Time_Diff","Time_Diff",800,0,800);
    m_h1_drift_z = new TH1F("Drift_Z","Drift_Z",500,0,50);
    m_h1_drift_xy = new TH1F("Drift_XY","Drift_XY",500,0,50);
    m_h1_angle = new TH1F("Angle","Angle",90,0,90);
    m_h1_sum_max = new TH1F("SumMaxAmp","SumMaxAmp",5000,0,5000);
    //m_h1_dedx = new TH1F("DEDX","DEDX",);

    m_h2_ene_vs_hits = new TH2F("Energy-Hits","Energy-Hits"
        ,5000,0,1000,60,0,60);
    m_h2_start_pos = new TH2F("Start_XY","Start_XY"
        ,60,0,60,60,0,60);




  }

  void draw(std::string const& v="viewer.root"){
    //auto* file = new TFile(v.c_str(),"recreate");
    //file->cd();
    //m_h1_spectrum->Write();
    m_h1_spectrum->Write();
    m_h1_ratio->Write();
    m_h1_time_diff->Write();
    m_h1_drift_z->Write();
    m_h1_drift_xy->Write();
    m_h1_angle->Write();
    m_h1_tot_hits->Write();
    m_h1_x_hits->Write();
    m_h1_y_hits->Write();
    m_h1_sum_max->Write();
    m_h2_ene_vs_hits->Write();
    m_h2_start_pos->Write();
    //file->Write(); file->Close();
  }



private:
  TH1F* m_h1_spectrum ,*m_h1_ratio,* m_h1_time_diff
    ,* m_h1_drift_z,* m_h1_drift_xy,* m_h1_angle;
  TH1I* m_h1_tot_hits,* m_h1_x_hits,* m_h1_y_hits;
  TH1F* m_h1_sum_max;

  TH2F* m_h2_ene_vs_hits,* m_h2_start_pos;
   



  

};
//---------------------------------------------------------------------



using std::cout; using std::endl; using std::string; using std::vector;
int main(int argc, char* argv[]){
  signal_handler<baseline,baseline> handler;
  if (argc==1){
    std::cout<<"Usage: ./main baseline.dat signal.dat"<<std::endl;
  }



  std::string fname, fname_signal;

  if (argc>=2) fname = argv[1];
  //std::string fname_signal = "/home/wangying/work/alphabetatpc_v2/data/20240605165343_signal_Sr90_1min.dat";
  if (argc>=3) fname_signal = argv[2];
  //
  handler.baseline_name(fname);

  //handler.baseline_viewer(true);
  handler.baseline_viewer(true);
  handler.draw_baseline(true);
  handler.get_baseline();
  info_out("baseline unpack finish");
 

  handler.file_name(fname_signal);
  handler.signal_viewer(true);
  handler.draw_signal(true);
  handler.get_signal(); //TODO
  info_out("signal unpack finish");




#ifdef raw_to_raw
  {

    auto* file_out = new TFile("raw_reco.root","recreate");
   
    adc_data_t data_frame;
    auto* raw_tree = new TTree("data","data");
    data_frame.link_in_tree(raw_tree);
   
    rec_data reconstructor;
    reconstructor.eledata_handlder = &handler;
    auto* reco_tree = new TTree("reco_data","reco_data");
    reconstructor.link_out_tree(reco_tree);
    reconstructor.init_histo();
    std::vector<TGraph*> graphs;
   
   
   
    size_t index_01 = 0;
    {
      boost::timer::auto_cpu_timer progress;
      for(auto&& [x,y] : handler.m_signal_table){
        data_frame.clear();
        data_frame.n_hits = y.size();
        data_frame.fec.resize(y.size());
        data_frame.chip.resize(y.size());
        data_frame.chn.resize(y.size());
        data_frame.event_id = x;
        //info_out(x);
        for (auto&& [y0,y1] : y){
          data_frame.channels.emplace_back(y0);
          for (auto&& y10 : y1) data_frame.adcs.emplace_back(y10);
          auto cname = util::config::channels_map[y0];
          if (cname[0]=='A'){
            data_frame.sum_adc.emplace_back(0);
            data_frame.max_adc.emplace_back(0);
            data_frame.max_point.emplace_back(0.);
            data_frame.pixel_x.emplace_back(-1);
            data_frame.pixel_y.emplace_back(-1);
          }else if(cname[0]=='X'){
            data_frame.sum_adc.emplace_back(
                std::accumulate(std::begin(y1),std::end(y1),(uint32_t)0));
            data_frame.summax_adc += data_frame.sum_adc.back();
            auto max_iter = std::max_element(std::begin(y1),std::end(y1));
            data_frame.max_adc.emplace_back(*max_iter);
            data_frame.max_point.emplace_back((float)std::distance(std::begin(y1),max_iter));
            data_frame.pixel_x.emplace_back(std::atoi(cname.substr(1).c_str()));
            data_frame.pixel_y.emplace_back(0);
          }else if(cname[0]=='Y'){
            data_frame.sum_adc.emplace_back(
                std::accumulate(std::begin(y1),std::end(y1),(uint32_t)0));
            data_frame.summax_adc += data_frame.sum_adc.back();
            auto max_iter = std::max_element(std::begin(y1),std::end(y1));
            data_frame.max_adc.emplace_back(*max_iter);
            data_frame.max_point.emplace_back((float)std::distance(std::begin(y1),max_iter));
            data_frame.pixel_y.emplace_back(std::atoi(cname.substr(1).c_str()));
            data_frame.pixel_x.emplace_back(0);
          }
        }
        if (index_01%1000==0){
          TGraph* gr = new TGraph();
          for(size_t i=1; auto&& x : data_frame.adcs) gr->SetPoint(i,i++,x);
          graphs.emplace_back(gr);
          std::string name_buf = std::to_string(index_01);
          auto* folder = new TFolder(name_buf.c_str(),name_buf.c_str());
          for (decltype(data_frame.n_hits) i=0; i<data_frame.n_hits; ++i){
            auto wave = data_frame.get_wave(i);
            auto* gr = util::to_root_graph(wave.begin(),wave.end());
            std::stringstream sstr; sstr<<index_01<<"_"<<i;
            gr->SetName(sstr.str().c_str()); gr->SetTitle(sstr.str().c_str());
            folder->Add(gr);
          }
          folder->Write();
   
   
          
   
   
          //info_out(data_frame.adcs.size());
        }
        data_frame.fill(raw_tree);
        reconstructor.reco(data_frame);
        reco_tree->Fill();
   
        //if (index_01==20000){
        //  for(int i=0; i<data_frame.n_hits; ++i){
        //    auto wave = data_frame.get_wave(i);
        //    auto* gr = new TGraph();
        //    for(size_t i=1; auto&& x : wave) gr->SetPoint(i,i++,x);
        //    graphs.emplace_back(gr);
        //  }
        //}
   
        if (index_01%10000==0) info_out("<========");
        //if (index_01%10000==0 && index_01!=0) {info_out("<========"); break;}
        index_01++;
      }
    }
    info_out(raw_tree->GetEntries());
    file_out->cd();
   
    //for (auto&& x : graphs) x->Write();
    reco_tree->Write();
    raw_tree->Write();
    file_out->Write(); file_out->Close();
    //reconstructor.draw("viewer.root");
    }
#endif

  return 0;
}
