#ifndef PKRV_HYBRID_MAP_H_
#define PKRV_HYBRID_MAP_H_

#include <vector>
#include <unordered_map>

namespace pkrv {

class HybridMap {

public:

  void reserve(size_t count) {
    flat_.resize(count + 1, -1);
    max_flat_id_ = count;
  }
  
  inline void set(size_t id, size_t index) {
    if ((id > 0) && (id <= max_flat_id_))
      flat_[id] = index;
    else sparse_[id] = index;
  }
  
  inline size_t get(size_t id) const {
    if ((id > 0) && (id <= max_flat_id_)) {
      size_t idx = flat_[id];
      if (idx != -1) return idx;
    } auto it = sparse_.find(id);
    return (it != sparse_.end()) ? it->second : -1;
  }

private:

  std::vector<size_t> flat_;
  std::unordered_map<size_t, size_t> sparse_;
  size_t max_flat_id_ = 0;


};

}  //namespace pkrv

#endif  // PKRV_HYBRID_MAP_H_