// Copyright(c) 2026 Stepan Markov - <markov92@rambler.ru>

#ifndef PKRV_MODEL_H_
#define PKRV_MODEL_H_

#include <unordered_map>
#include <string>
#include <utils/hybrid_map.h>

// DOD owner

namespace pkrv {

class Model {
public:

  Model() = default;

  struct ElementBlock {
    std::string type_name;
    std::vector<size_t> element_ids;
    std::vector<size_t> connectivity;
  };
  
  void AddNodes(
    const std::string& node_set_name,
    std::vector<size_t>&& ids,
    std::vector<double>&& posX,
    std::vector<double>&& posY,
    std::vector<double>&& posZ);
  
  inline const auto& blocks       () const { return blocks_       ;}
  inline const auto& node_set     () const { return node_set_     ;}
  inline const auto& element_set  () const { return element_set_  ;}
  inline const auto& posX         () const { return posX_         ;}
  inline const auto& posY         () const { return posY_         ;}
  inline const auto& posZ         () const { return posZ_         ;}
  inline const auto& index_to_id  () const { return index_to_id_  ;}
  inline const auto& id_to_index  () const { return id_to_index_  ;}

private:

  std::vector<double> posX_, posY_, posZ_;
  std::unordered_multimap<std::string, ElementBlock>    blocks_;
  std::unordered_map<std::string, std::vector<size_t>>  node_set_, element_set_;
  
  HybridMap id_to_index_;
  std::vector<size_t> index_to_id_;

};



}  //namespace pkrv

#endif  // PKRV_MODEL_H_