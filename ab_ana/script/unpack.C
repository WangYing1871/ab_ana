#define info_out(X) std::cout<<"==> "<<__LINE__<<" "<<#X<<" |"<<(X)<<"|\n"
#include "util.hpp"
#include "data_strcut_cint.h"
ClassImp(entry_new)

struct parse_base_t{
  std::string m_name;

public:
  parse_base_t() = default;
  parse_base_t(std::string const& v):m_name(v) {}
  ~parse_base_t() noexcept = default;

  virtual bool parse(char*&, char* const&) = 0;
  virtual void clear() {}
  virtual std::ostream& display(std::ostream& os = std::cout) const {return os;}
};
struct waveform_by_entry;
struct waveform_by_entry : public parse_base_t{
  typedef waveform_by_entry self_t;
  typedef parse_base_t base_t;
  typedef waveform_pack_head head_t;
  typedef waveform_pack_body body_t;
  typedef waveform_pack_tail tail_t;

  waveform_by_entry() = default;
  waveform_by_entry(std::string const&);
  ~waveform_by_entry() noexcept = default;


  virtual bool parse(char*& iter, char* const& end) override;
  virtual void clear() override {m_unit.bodys.clear(); m_unit.heads.clear(); m_unit.tails.clear();}
  virtual std::ostream& display(std::ostream& os = std::cout) const override;

public:
  inline void set_store(entry_new& ref) {m_store_ref=std::addressof(ref);}
  inline void set_tree(TTree* ref) {m_tree_ref=ref;}


private:
  bool valid(head_t const&);
  bool valid(body_t const&);
  bool valid(tail_t const&);

private:
  entry_new* m_store_ref=nullptr;
  TTree* m_tree_ref=nullptr;
  struct unit_t{
    std::vector<head_t> heads;
    std::list<body_t> bodys;
    std::vector<tail_t> tails;
  };
  unit_t m_unit;
  void store();
  constexpr static uint8_t const cs_start_tag = 0x5a;

  std::size_t board_no = 1;
public:
  inline uint32_t get_event_id(head_t const& head) const{ return head.event_id;}
  inline uint8_t get_fec_id(head_t const& v) const{ return v.fec_id&0x3F;}
  inline uint8_t get_fec_id(body_t const& v) const{ return v.reserved0;}

  uint64_t get_timestamp(head_t const&) const;
  inline uint8_t get_channel_id(body_t const& v) const{ return v.channel_id & 0x7F; }

public:
  static constexpr std::index_sequence<8,8> const glb_id_idx={};
};
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
  m_store_ref->event_ids.resize(m_unit.heads.size());
  std::transform(std::begin(m_unit.heads),std::end(m_unit.heads),std::begin(m_store_ref->event_ids)
      ,[&](head_t const& v){return this->get_event_id(v);});

  m_store_ref->fec_ids.resize(m_unit.heads.size());
  std::transform(std::begin(m_unit.heads),std::end(m_unit.heads),std::begin(m_store_ref->fec_ids)
      ,[&](head_t const& v){return this->get_fec_id(v);});

  m_store_ref->global_ids.resize(m_unit.bodys.size());
  m_store_ref->adcs.resize(m_unit.bodys.size());
  std::size_t index=0;
  for (auto&& x : m_unit.bodys){
    //meta::encode(m_store_ref->global_ids[index],glb_id_idx,get_fec_id(x),get_channel_id(x));
    m_store_ref->global_ids[index] = ((uint16_t)get_fec_id(x)<<8) + (uint16_t)get_channel_id(x);
    m_store_ref->adcs[index].resize(1024);
    std::size_t j=0;
    for (auto&& y : x.adc) m_store_ref->adcs[index][j++] = (y&0x0FFF);
    index++;
  }
  m_tree_ref->Fill();
}

uint64_t waveform_by_entry::get_timestamp(head_t const& v) const{
  //__attribute__((unused)) v;
  (void)v;
  return 0;
}
bool waveform_by_entry::parse(char*& iter, char* const& end){
  using namespace util;
  auto const& parse_head = [&](head_t& head)->bool{
    read_int(head.start_tag,iter);
    read_int(head.package_size,iter);
    if ((head.package_size&0x1FFF)!=20) return false;
    read_int(head.fec_id,iter);
    read_int<uint64_t,6>(head.time_stamp,iter);
    read_int(head.event_id,iter);
    read_int(head.hit_channel_no,iter);
    read_int(head.reserved,iter);
    read_int(head.crc32,iter);
    return true;
  };

  auto const& parse_body = [&](body_t& bd)->bool{
    read_int(bd.start_tag,iter);
    read_int(bd.adc_package_size,iter);
    if((bd.adc_package_size&0x1FFF)!=2060 
        && (bd.adc_package_size&0x1FFF)!=14)
      return false;
    read_int(bd.reserved0,iter);
    read_int(bd.channel_id,iter);
    read_int(bd.reserved1,iter);
    for (auto&& x : bd.adc) read_int(x,iter);
    read_int(bd.reserved2,iter);
    read_int(bd.crc32,iter);
    return true;
  };

  auto const& parse_tail = [&](tail_t& tail)->bool{
    read_int(tail.start_tag,iter);
    read_int(tail.package_size,iter);
    if ((tail.package_size&0x1FFF)!=12) return false;
    read_int(tail.reserved,iter);
    read_int(tail.event_size,iter);
    read_int(tail.crc32,iter);
    return true;
  };

  enum class e_state : uint8_t{
    k_unknow
    ,k_have_head
    ,k_in_body
    ,k_have_tail
  };

  int head_count = 0;
  char* begin = iter;

  e_state stream_state = e_state::k_unknow;
  char* back = iter;
  while(std::distance(iter,end)>0){
    if(iter[0]==cs_start_tag){
      back = iter;
      if ((stream_state==e_state::k_unknow 
           || stream_state==e_state::k_have_head)){
        m_unit.heads.resize(m_unit.heads.size()+1);
        auto& h_ref = m_unit.heads.back();
        if (parse_head(h_ref) && valid(h_ref)){
          head_count++;
          stream_state=e_state::k_have_head;
          continue;
        }else{
          iter = back;
          m_unit.heads.erase(std::prev(m_unit.heads.end()));
        }
      }
        
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
          || stream_state==e_state::k_in_body
          || stream_state==e_state::k_have_tail)){
        m_unit.tails.resize(m_unit.tails.size()+1);
        auto& t_ref = m_unit.tails.back();
        if (parse_tail(t_ref) && valid(t_ref)){
          stream_state=e_state::k_have_tail;
          head_count--;
          if (head_count==0) { 
            //display(std::cout);
            store(); clear();
            stream_state=e_state::k_unknow;
          }
          continue;
        }else{
          iter = back;
          m_unit.tails.erase(std::prev(m_unit.tails.end()));
        }
      }
    }else {
      iter++;
    }
  }
  return true;
  
}

std::ostream& waveform_by_entry::display(std::ostream& os) const{
  os
    <<m_unit.heads.size()<<"\n";
    os<<"\t";
    for (auto&& x : m_unit.heads) os<<(int)get_fec_id(x)<<" ";
  os
    <<"\n"
    <<m_unit.bodys.size()<<"\n";
    for (auto&& x : m_unit.bodys) os<<(int)get_fec_id(x)<<" ";
  os
    <<"\n"
    <<m_unit.tails.size()<<"\n";

  //os<<std::hex
  //  <<(uint32_t)m_unit.head.start_tag<<" "
  //  <<(uint32_t)m_unit.head.package_size<<" "
  //  <<(uint32_t)m_unit.head.fec_id<<" "
  //  <<(uint64_t)m_unit.head.time_stamp<<" "
  //  <<(uint32_t)m_unit.head.event_id<<" "
  //  <<(uint32_t)m_unit.head.hit_channel_no<<" "
  //  <<(uint32_t)m_unit.head.reserved<<" "
  //  <<(uint32_t)m_unit.head.crc32<<" "
  //  <<"\n=====================\n"
  //  <<(uint32_t)m_unit.bodys.size()<<" "
  //  <<(uint32_t)m_unit.bodys.begin()->start_tag<<" "
  //  <<(uint32_t)m_unit.bodys.begin()->adc_package_size<<" "
  //  <<(uint32_t)m_unit.bodys.begin()->reserved0<<" "
  //  <<(uint32_t)m_unit.bodys.begin()->channel_no<<" "
  //  <<(uint32_t)m_unit.bodys.begin()->reserved1<<" "
  //  <<(uint32_t)m_unit.bodys.begin()->reserved2<<" "
  //  <<(uint32_t)m_unit.bodys.begin()->crc32<<" "
  //  <<"\n=====================\n"
  //  <<(uint32_t)m_unit.tail.start_tag<<" "
  //  <<(uint32_t)m_unit.tail.package_size<<" "
  //  <<(uint32_t)m_unit.tail.reserved<<" "
  //  <<(uint32_t)m_unit.tail.event_size<<" "
  //  <<(uint32_t)m_unit.tail.crc32<<" "
  //  <<std::dec
  //  << "\n";
  return os;
}

  
int unpack(){
  auto argv_map = util::read_argv("unpack");
  std::string dat_name=argv_map["filename"];
  std::string entry_out_file = dat_name.substr(
      0,dat_name.find_last_of("."))+"_entry.root";
  
  TFile* fout = new TFile(entry_out_file.c_str(),"recreate");
  TTree* data_tree = new TTree("CollectionTree","CollectionTree");
  entry_new entry_buffer;
  data_tree->Branch("data",std::addressof(entry_buffer));
  
  std::ifstream fin(dat_name.c_str(),std::ios::binary);
  fin.seekg(0,std::ios_base::end);
  size_t fsz = fin.tellg();
  fin.seekg(0,std::ios_base::beg);
  char* data = new char[fsz];
  fin.read(data,fsz);
  char* iter_beg = data;
  fin.close();
  waveform_by_entry wf;

  wf.set_store(entry_buffer);
  wf.set_tree(data_tree);

  wf.parse(iter_beg,iter_beg+fsz);
  fout->cd();
  data_tree->Write(); 
  fout->Write(); fout->Close(); 
  delete[] data;
  

  return 0;


}
