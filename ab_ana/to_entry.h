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

  std::unordered_map<int,std::pair<float,float>> m_baseline;
  //std::function<bool()>


  to_entry() = default;
  ~to_entry() = default;

  void run();


};

#endif
