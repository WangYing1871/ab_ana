#ifndef data_strcut_cint_H
#define data_strcut_cint_H 1 

#include <array>
#include <list>
#include <vector>

struct waveform_pack{
  uint16_t head_tag = 0;
  uint16_t package_size;
  uint8_t type;
  uint32_t evt_id;
  uint8_t trigger_idx;
  uint16_t channel_idx;
  uint16_t adc[1024];
  uint32_t crc32;
  uint16_t tail_tag=0;

  waveform_pack& operator=(waveform_pack&) = default;
};

struct tq_pack{
  uint16_t head_tag = 0;
  uint16_t package_size;
  uint8_t type;
  uint32_t evt_id;
  uint8_t trigger_idx;
  uint16_t channel_idx;
  uint16_t amp;
  uint32_t crc32;
  uint16_t half_time;
  uint16_t peak_time;
  uint16_t tail_tag = 0;
};

struct entry{
  //typedef std::array<uint16_t,1024> adcs_t;
  uint16_t m_hits;
  std::vector<uint16_t> m_channels;
  //std::vector<adcs_t> m_adcs;
  std::vector<std::vector<uint16_t>> m_adcs;
  std::vector<uint32_t> m_sum_adc;
  std::vector<uint16_t> m_max_adc;
  std::vector<uint16_t> m_max_point;
  uint32_t m_sum_max_adc = 0;

  entry& operator=(entry&) = default;

};

struct collection_tree{
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

  collection_tree& operator=(collection_tree&) = default;
};


#endif
