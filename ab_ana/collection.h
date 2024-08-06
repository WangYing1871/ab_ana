#ifndef collection_H
#define collection_H 1 

#include <functional>
#include <string>
#include "data_strcut_cint.h"

struct collection{
  std::string m_in_file;
  std::string m_out_file;

  collection_tree m_data;

  typedef std::vector<uint16_t> waveform_t;

  std::function<bool(waveform_t const&)> m_wave_filter;


public:
  int init();
  int run();
  int finalize();

};

#endif
