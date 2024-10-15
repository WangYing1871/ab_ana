#ifndef unpack_H
#define unpack_H 1 
#include <vector>
#include "Rtypes.h"
struct entry_new{
public:
  entry_new() = default;
  virtual ~entry_new() = default;
  std::vector<uint32_t> event_ids;
  std::vector<uint8_t> fec_ids;
  std::vector<uint8_t> hit_channel_nos;
  std::vector<uint64_t> timestamp;
  std::vector<uint16_t> global_ids;
  std::vector<std::vector<uint16_t>> adcs;
  ClassDef(entry_new,1)
};
#endif
