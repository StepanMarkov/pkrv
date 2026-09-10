
#include "model.h"

namespace pkrv {

void Model::AddNodes(
  const std::string& node_set_name,
  std::vector<size_t>&& ids,
  std::vector<double>&& posX,
  std::vector<double>&& posY,
  std::vector<double>&& posZ) {

  const size_t old_node_count = index_to_id_.size();
  const size_t add_node_count = ids.size();
  const size_t new_node_count = old_node_count + add_node_count;
  
  if (old_node_count == 0) {
    posX_ = posX;
    posY_ = posY;
    posZ_ = posZ;
    index_to_id_ = ids;
  }
  else {
    posX_.reserve(new_node_count);
    posY_.reserve(new_node_count);
    posZ_.reserve(new_node_count);
    index_to_id_.reserve(new_node_count); 
    
    posX_.insert(posX_.end(), posX.begin(), posX.end());
    posY_.insert(posY_.end(), posY.begin(), posY.end());
    posZ_.insert(posZ_.end(), posZ.begin(), posZ.end());
    index_to_id_.insert(index_to_id_.end(), ids.begin(), ids.end());
  }
  
  if (!node_set_name.empty()) {
    std::vector<size_t> nodes(add_node_count);
    for (size_t node = 0; node < add_node_count; ++node)
      nodes[node] = old_node_count + node;
    node_set_.insert({ node_set_name, std::move(nodes) });
  }
  
  id_to_index_.reserve(new_node_count);
  for (size_t node = old_node_count; node < new_node_count; ++node)
    id_to_index_.set(index_to_id_[node], node);

}

} // namespace pkrv