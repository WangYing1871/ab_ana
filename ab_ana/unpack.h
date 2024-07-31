#ifndef unpack_H
#define unpack_H 1 

#include <iostream>
#include <string>
#include <filesystem>

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
