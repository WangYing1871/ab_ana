#ifndef unpack_H
#define unpack_H 1 

#include <iostream>
#include <string>
#include <filesystem>
#include <list>
class TTree;

#include "data_strcut_cint.h"

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

struct waveform : public parse_base_t{
  typedef waveform self_t;
  typedef parse_base_t base_t;

  static constexpr uint16_t const s_head_tag = 0x55aa;
  static constexpr uint16_t const s_tail_tag = 0x5aa5;

  inline bool head_valid() const {return m_data.head_tag==s_head_tag;}
  inline bool tail_valid() const {return m_data.tail_tag==s_tail_tag;}

private:
public:
  waveform_pack m_data;

private:
  bool is_compress_adc=false;

public:
  waveform() = default;
  waveform(std::string const&);
  ~waveform()noexcept = default;

  virtual void clear() override{ for(auto&& x : m_data.adc) x = 0; }
  virtual bool parse(char*& iter, char* const& end) override;
  virtual std::ostream& display(std::ostream& os = std::cout) const override;

  inline size_t get_channel_id() const{ return m_data.channel_idx&0x00FF; }
  inline size_t get_event_id() const {return (m_data.evt_id<<8)+m_data.trigger_idx;}

  void corr();
  
};

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
  virtual void clear() override {m_unit.bodys.clear();}
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
    head_t head;
    std::list<body_t> bodys;
    tail_t tail;
  };
  unit_t m_unit;
  void store();
  constexpr static uint8_t const cs_start_tag = 0x5a;

public:
  inline uint32_t get_event_id() {return m_unit.head.event_id;}
  inline uint8_t get_fec_id() {return m_unit.head.fec_id&0x3f;}
  inline uint8_t get_hit_channel_no() const {return m_unit.head.hit_channel_no&0x3f; }
  uint64_t get_timestamp () const;
};

struct tq : public parse_base_t{
  typedef tq self_t;
  typedef parse_base_t base_t;

  static constexpr uint16_t const s_head_tag = 0x55aa;
  static constexpr uint16_t const s_tail_tag = 0x5aa5;
  inline bool head_valid() const {return m_data.head_tag==s_head_tag;}
  inline bool tail_valid() const {return m_data.tail_tag==s_tail_tag;}

//private:
public:
  tq_pack m_data;

public:
  tq() = default;
  tq(std::string const& v) : parse_base_t(v) {}
  ~tq() noexcept = default;


  bool valid() const{
    return m_data.package_size==sizeof(m_data) && head_valid() && tail_valid(); }

  virtual std::ostream& display(std::ostream& os = std::cout) const override;

  virtual bool parse(char*& iter, char* const& end) override;
  size_t get_channel_id() const{ return m_data.channel_idx&0x00FF; }
  size_t get_event_id() const {return m_data.evt_id;} //FIXME
};

struct unpacking_tool{
  std::string m_in_file;
  std::string m_out_file;
  parse_base_t* m_parser;

public:
  void unpack();
  void unpack_fast();

private:
  static size_t s_data_buf_size;
};




#endif
