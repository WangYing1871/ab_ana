#include <fstream>
#include <numeric>
#include <atomic>
#include <thread>
#include <chrono>
#include <iomanip>
#include <stdexcept>

//#include <boost/timer/timer.hpp>

#include "data_strcut_cint.h"
#include "unpack.h"
#include "util.hpp"

#include "TFile.h"
#include "TTree.h"



//---------------------------------------------------------------------
waveform_by_entry::waveform_by_entry(std::string const& n):parse_base_t(n){
}

/* TODO  how to write valid ??*/ 
bool waveform_by_entry::valid(head_t const& head){
  //if (head.start_tag!=cs_start_tag) return false;
  //auto ps = head.package_size&0x1FFF;
  //if ((head.package_size&0x1FFF) != 20) return false;
  //if (head.package.crc32... /* TODO*/)
  return true;
}


bool waveform_by_entry::valid(body_t const& body){
  //if (body.start_tag!=cs_start_tag) return false;
  //if(body.adc_package_size&0x1FFF!=2060 
  //    && body.adc_package_size&0x1FFF!=14) return false;
  return true;
}

bool waveform_by_entry::valid(tail_t const& tail){
  //if (tail.start_tag!=cs_start_tag) return false;
  //if((tail.package_size&0x1FFF)!=12) return false;
  return true;
}

void waveform_by_entry::store(){
  if (!m_store_ref || !m_tree_ref) return;
  m_store_ref->event_id = get_event_id();
  m_store_ref->fec_id = get_fec_id();
  m_store_ref->hit_channel_no = get_hit_channel_no();
  m_store_ref->timestamp = get_timestamp();

  m_store_ref->channel_ids.resize(m_unit.bodys.size());
  m_store_ref->adcs.resize(m_unit.bodys.size());
  for (std::size_t i = 0; auto&& x : m_unit.bodys){
    m_store_ref->adcs[i].resize(1024);
    for (std::size_t j=0; auto&& y : x.adc) m_store_ref->adcs[i][j++] = (y&0x0FFF);
    m_store_ref->channel_ids[i++] = (x.channel_no&0x7F);

  }
  m_tree_ref->Fill();
}

uint64_t waveform_by_entry::get_timestamp() const{
  return 0;
}

bool waveform_by_entry::parse(char*& iter, char* const& end){
  using namespace util;
  auto const& parse_head = [&]()->bool{
    read_int(m_unit.head.start_tag,iter);
    //if (m_unit.head.start_tag!=cs_start_tag) return false;
    read_int(m_unit.head.package_size,iter);
    if ((m_unit.head.package_size&0x1FFF)!=20) return false;
    read_int(m_unit.head.fec_id,iter);
    read_int<uint64_t,6>(m_unit.head.time_stamp,iter);
    read_int(m_unit.head.event_id,iter);
    read_int(m_unit.head.hit_channel_no,iter);
    read_int(m_unit.head.reserved,iter);
    read_int(m_unit.head.crc32,iter);
    return true;
  };

  auto const& parse_body = [&](body_t& bd)->bool{
    read_int(bd.start_tag,iter);
    //if (bd.start_tag!=cs_start_tag) return false;
    read_int(bd.adc_package_size,iter);
    if((bd.adc_package_size&0x1FFF)!=2060 
        && (bd.adc_package_size&0x1FFF)!=14)
      return false;
    read_int(bd.reserved0,iter);
    read_int(bd.channel_no,iter);
    read_int(bd.reserved1,iter);
    for (auto&& x : bd.adc) read_int(x,iter);
    read_int(bd.reserved2,iter);
    read_int(bd.crc32,iter);
    return true;
  };

  auto const& parse_tail = [&]()->bool{
    auto& ref = m_unit.tail;
    read_int(ref.start_tag,iter);
    read_int(ref.package_size,iter);
    if ((ref.package_size&0x1FFF)!=12) return false;
    read_int(ref.reserved,iter);
    read_int(ref.event_size,iter);
    read_int(ref.crc32,iter);
    return true;
  };

  enum class e_state : uint8_t{
    k_unknow
    ,k_have_head
    ,k_in_body
    ,k_have_tail
  };

  e_state stream_state = e_state::k_unknow;
  char* back = iter;
  while(std::distance(iter,end)>0){
    if(iter[0]==cs_start_tag){
      back = iter;
      if ((stream_state==e_state::k_unknow 
           /*|| stream_state==e_state::k_in_body*/)
          && parse_head() 
          && valid(m_unit.head)){
        stream_state=e_state::k_have_head;
        continue;
      }else iter = back;


      back = iter;
      if (stream_state==e_state::k_have_head
          || stream_state==e_state::k_in_body){
        char* back = iter;
        m_unit.bodys.resize(m_unit.bodys.size()+1);
        auto& b_ref = m_unit.bodys.back();
        if (parse_body(b_ref) && valid(b_ref)){
          stream_state=e_state::k_in_body;
          continue;
        }
        else{
          iter = back;
          m_unit.bodys.erase(std::prev(m_unit.bodys.end()));
        }
      }

      back = iter;
      if ((stream_state==e_state::k_have_head
          || stream_state==e_state::k_in_body)
          && parse_tail() && valid(m_unit.tail)){
        stream_state=e_state::k_unknow;
        store();
        clear();
      }else iter = back;
    }else iter++;
  }
  return true;
  
}

std::ostream& waveform_by_entry::display(std::ostream& os) const{
  os<<std::hex
    <<(uint32_t)m_unit.head.start_tag<<" "
    <<(uint32_t)m_unit.head.package_size<<" "
    <<(uint32_t)m_unit.head.fec_id<<" "
    <<(uint64_t)m_unit.head.time_stamp<<" "
    <<(uint32_t)m_unit.head.event_id<<" "
    <<(uint32_t)m_unit.head.hit_channel_no<<" "
    <<(uint32_t)m_unit.head.reserved<<" "
    <<(uint32_t)m_unit.head.crc32<<" "
    <<"\n=====================\n"
    <<(uint32_t)m_unit.bodys.size()<<" "
    <<(uint32_t)m_unit.bodys.begin()->start_tag<<" "
    <<(uint32_t)m_unit.bodys.begin()->adc_package_size<<" "
    <<(uint32_t)m_unit.bodys.begin()->reserved0<<" "
    <<(uint32_t)m_unit.bodys.begin()->channel_no<<" "
    <<(uint32_t)m_unit.bodys.begin()->reserved1<<" "
    <<(uint32_t)m_unit.bodys.begin()->reserved2<<" "
    <<(uint32_t)m_unit.bodys.begin()->crc32<<" "
    <<"\n=====================\n"
    <<(uint32_t)m_unit.tail.start_tag<<" "
    <<(uint32_t)m_unit.tail.package_size<<" "
    <<(uint32_t)m_unit.tail.reserved<<" "
    <<(uint32_t)m_unit.tail.event_size<<" "
    <<(uint32_t)m_unit.tail.crc32<<" "
    <<std::dec
    << "\n";
  return os;
}
//---------------------------------------------------------------------


waveform::waveform(std::string const& n) : parse_base_t(n){
  clear();
}
bool waveform::parse(char*& iter, char* const& end){
  using namespace util;
  read_int(this->m_data.head_tag,iter);
  if (!this->head_valid()) return false;
  auto data_restore = *this; this->clear(); auto iter_restore = iter;
  read_int(this->m_data.package_size,iter);
  read_int(this->m_data.type,iter);
  read_int(this->m_data.evt_id,iter);
  read_int(this->m_data.trigger_idx,iter);
  read_int(this->m_data.channel_idx,iter);
  for (auto&& x : this->m_data.adc) read_int(x,iter);
  read_int(this->m_data.crc32,iter);
  read_int(this->m_data.tail_tag,iter);
  if (!this->tail_valid()) {*this = data_restore; iter=iter_restore; return false;}
  return true; };

std::ostream& waveform::display(std::ostream& os) const{
  return os<<std::hex<<"==================================================\n"
    <<"  head_tag\t"<<(int)m_data.head_tag<<"\n"
    <<"  package_size\t"<<(int)m_data.package_size<<"\n"
    <<"  type\t"<<(int)m_data.type<<"\n"
    <<"  evt_id\t"<<(int)m_data.evt_id<<"\n"
    <<"  trigger_idx\t"<<(int)m_data.trigger_idx<<"\n"
    <<"  channel_idx\t"<<(int)m_data.channel_idx<<"\n"
    <<"  crc32\t"<<(int)m_data.crc32<<"\n"
    <<"  tail_tag\t"<<(int)m_data.tail_tag<<"\n"
    <<std::dec
    ; }

void waveform::corr() {
  m_data.channel_idx=get_channel_id(); m_data.evt_id=get_event_id();
  for (auto&& x : m_data.adc) x = x&0x0FFF;
}

bool tq::parse(char*& iter,char* const& end){
  using namespace util;
  read_int(this->m_data.head_tag,iter);
  if (!this->head_valid()) return false;
  auto data_restore = *this;auto iter_restore = iter;
  read_int(this->m_data.package_size,iter);
  read_int(this->m_data.type,iter);
  read_int(this->m_data.evt_id,iter);
  read_int(this->m_data.trigger_idx,iter);
  read_int(this->m_data.channel_idx,iter);
  read_int(this->m_data.amp,iter);
  read_int(this->m_data.half_time,iter);
  read_int(this->m_data.peak_time,iter);

  read_int(this->m_data.crc32,iter);
  read_int(this->m_data.tail_tag,iter);
  if (!this->tail_valid()) {*this = data_restore; iter=iter_restore; return false;}
  return true; }
 
std::ostream& tq::display(std::ostream& os) const{
  return os<<std::hex<<"==================================================\n"
    <<"  head_tag\t"<<(int)m_data.head_tag<<"\n"
    <<"  package_size\t"<<(int)m_data.package_size<<"\n"
    <<"  type\t"<<(int)m_data.type<<"\n"
    <<"  evt_id\t"<<(int)m_data.evt_id<<"\n"
    <<"  trigger_idx\t"<<(int)m_data.trigger_idx<<"\n"
    <<"  channel_idx\t"<<(int)m_data.channel_idx<<"\n"
    <<"  amp\t"<<(int)m_data.amp<<"\n"
    <<"  half_time\t"<<(int)m_data.half_time<<"\n"
    <<"  peak_time\t"<<(int)m_data.peak_time<<"\n"
    <<"  crc32\t"<<(int)m_data.crc32<<"\n"
    <<"  tail_tag\t"<<(int)m_data.tail_tag<<"\n"
    <<std::dec
    ; }

//---------------------------------------------------------------------
size_t unpacking_tool::s_data_buf_size(32*1024*1024);

void unpacking_tool::unpack(){
  if (!m_parser) return;
  auto* out_file = new TFile(m_out_file.c_str(),"recreate");
  auto* tree = new TTree("raw","raw");
  waveform_pack data_strcut_buf;
  tree->Branch("data",&data_strcut_buf);

  //boost::timer::auto_cpu_timer progress;
  std::ifstream fin(m_in_file.c_str(),std::ios_base::binary);
  fin.seekg(0,std::ios_base::end);
  size_t fsz = fin.tellg();
  fin.seekg(0,std::ios_base::beg);
  size_t psz = unpacking_tool::s_data_buf_size;
  char* bytes_begin = new char[psz];
  size_t unknow_number = 0;
  out_file->cd();

  std::atomic<bool> is_file_end(false);
  //std::thread slider([&](){
  //    while(!is_file_end.load()){
  //      std::this_thread::sleep_for(std::chrono::milliseconds(500));
  //      float schedule = fin.tellg()/(double)fsz;
  //      std::cout<<"\r\tunpack\t";
  //      for (size_t i=0; i<100*schedule; ++i) std::cout.put('=');
  //      for (size_t i=100*schedule; i<100; ++i) std::cout.put('-');
  //      std::cout<<"\t"<<std::setprecision(4)
  //        <<schedule*100<<" %"<<std::flush;}
  //    std::cout<<std::endl;
  //    });
  //slider.detach();

  while(!fin.eof()){
    std::vector<uint8_t> unknow_bytes;
    fin.read(bytes_begin+unknow_number,psz-unknow_number);
    size_t end = fin.gcount()+unknow_number;
    size_t pack_get = 0;
    for (auto iter = bytes_begin; std::distance(bytes_begin,iter)<end;){
      if(m_parser->parse(iter,nullptr)){
        pack_get++;
        auto* ptr = dynamic_cast<waveform*>(m_parser);
        ptr->corr();
        data_strcut_buf = ptr->m_data;
        tree->Fill();
      }else unknow_bytes.emplace_back(iter[-2]), unknow_bytes.emplace_back(iter[-1]);
    }
    if (pack_get==0)
      throw std::invalid_argument("32MB data hasn't get any package. please check.");
    unknow_number = unknow_bytes.size();
    std::copy(std::begin(unknow_bytes),std::end(unknow_bytes),bytes_begin);
  }
  is_file_end.store(true);
  delete[] bytes_begin;
  fin.close();
  tree->Write();
  out_file->Close(); out_file->Write();
}


void unpacking_tool::unpack_fast(){
  //boost::timer::auto_cpu_timer progress;
  if (!m_parser) return;
  std::ifstream fin(m_in_file.c_str(),std::ios_base::binary);
  fin.seekg(0,std::ios_base::end);
  size_t fsz = fin.tellg();
  char* file_buf = new char[fsz];
  fin.seekg(0,std::ios_base::beg);
  for (auto iter = file_buf; (size_t)std::distance(file_buf,iter)<fsz; ){
    if (m_parser->parse(iter,nullptr)){
        std::cout<<"2\n";
    }
  }
  fin.close();
  delete[] file_buf;
}



