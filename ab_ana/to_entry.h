#ifndef to_entry_H
#define to_entry_H 1 
#include <string>
#include <atomic>
#include <functional>
#include <unordered_map>
#include "data_strcut_cint.h"

struct to_entry{

  std::string m_in_file;
  std::string m_out_file;
  std::string m_out_pdf;
  //std::atomic<bool> _is_print_pdf(false);
  //std::atomic<bool> _is_del_baseline(false);
  bool _is_print_pdf = false;
  bool _is_del_baseline = false;

  std::unordered_map<uint16_t,std::pair<float,float>> m_baseline;
  std::unordered_map<uint16_t,entry> m_entry_map;
  //std::function<bool()>


  to_entry() = default;
  ~to_entry() = default;

  void run();

public:
  void is_print_pdf(bool v) {_is_print_pdf = v;}
  void out_pdf_name(std::string const& v) {m_out_pdf = v;}


};

#endif
