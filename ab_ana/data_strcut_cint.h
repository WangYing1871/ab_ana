#ifndef data_strcut_cint_H
#define data_strcut_cint_H 1 

#include <array>
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
  uint16_t m_hits;
  std::vector<uint16_t> m_channels;
  std::vector<uint16_t> adcs;
  std::vector<uint32_t> m_sum_adc;
  std::vector<uint16_t> m_max_adc;
  std::vector<uint16_t> m_max_point;
  uint32_t m_sum_max_adc;

  entry& operator=(entry&) = default;

};

#endif
