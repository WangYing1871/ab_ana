#include <fstream>
#include <numeric>

#include <boost/timer/timer.hpp>

#include "data_strcut_cint.h"
#include "unpack.h"
#include "util.hpp"

#include "TFile.h"
#include "TTree.h"

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


  
  boost::timer::auto_cpu_timer progress;
  std::ifstream fin(m_in_file.c_str(),std::ios_base::binary);
  size_t psz = unpacking_tool::s_data_buf_size;
  char* bytes_begin = new char[psz];
  size_t unknow_number = 0;
  while(!fin.eof()){
    std::vector<uint8_t> unknow_bytes;
    fin.read(bytes_begin+unknow_number,psz-unknow_number);
    size_t end = fin.gcount()+unknow_number;
    for (auto iter = bytes_begin; std::distance(bytes_begin,iter)<end;){
      if(m_parser->parse(iter,nullptr)){
        auto* ptr = dynamic_cast<waveform*>(m_parser);
        data_strcut_buf = ptr->m_data;
        tree->Fill();
      }else unknow_bytes.emplace_back(iter[-2]), unknow_bytes.emplace_back(iter[-1]);
    }
    unknow_number = unknow_bytes.size();
    std::copy(std::begin(unknow_bytes),std::end(unknow_bytes),bytes_begin);
  }
  delete[] bytes_begin;
  fin.close();
  out_file->cd(); tree->Write();
  out_file->Close(); out_file->Write();
}
void unpacking_tool::unpack_fast(){
  boost::timer::auto_cpu_timer progress;
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

