// Copyright(c) 2026 Stepan Markov - <markov92@rambler.ru>

#include "solver.h"
#include <elements/concept_battery.h>
#include <iostream>

namespace pkrv {

void Solver::Solve() {

  const size_t global_node_count = model_.index_to_id().size();
  
  auto& global_liters = this->result_.get(State::LITERS_DISTRIBURION);
  auto& global_rpds   = this->result_.get(State::RPDS);
  global_liters.resize(global_node_count, 0);
  global_rpds.resize(global_node_count, 0);

  for (const auto& [nset, type] : this->model_.armor_section()) {
    if ((type == "RPDS1") || (type == "RPDS2")) {
      const size_t value = (type == "RPDS2") ? 1 : 0;
      auto range = this->model_.node_set().equal_range(nset);
      for (auto iter = range.first; iter != range.second; ++iter) {
        const auto& nodes = iter->second;
        for (const size_t& node : nodes)
          global_rpds[node] = value;
      }
    }
  }


  for (const auto& [name, block] : this->model_.blocks()) {
    if (block.type_name == "MRLS6") {
      std::array<size_t, mris6::node_count> rpds = {};
      std::array<std::array<double, mris6::node_count>, mris6::node_count> distance = {};
      std::array<size_t, mris6::node_count> liters = {};
      const size_t element_count = block.element_ids.size();

      const size_t* const connect  = block.connectivity.data();
      const size_t* const elem_ids = block.element_ids.data();
      const double* const posx     = this->model_.posX().data();
      const double* const posy     = this->model_.posY().data();
      const double* const posz     = this->model_.posZ().data();

      for (size_t element = 0; element < element_count; ++element) {

        const size_t* const nodes = connect + mris6::node_count * element;

        for (size_t i = 0; i < mris6::node_count; ++i)
          rpds[i] = global_rpds[nodes[i]];

        for (size_t i = 0; i < mris6::node_count; ++i) {
          auto& distance_i = distance[i];
          const size_t node_i = nodes[i];
          const double posx_i = posx[node_i];
          const double posy_i = posy[node_i];
          const double posz_i = posz[node_i];

          distance_i = {};

          for (size_t j = 0; j < i; ++j)
            distance_i[j] = distance[j][i];

          for (size_t j = i + 1; j < mris6::node_count; ++j) {
            const size_t node_j = nodes[j];
            const double dx = posx[node_j] - posx_i;
            const double dy = posy[node_j] - posy_i;
            const double dz = posz[node_j] - posz_i;
            distance_i[j] += dx * dx + dy * dy + dz * dz;
          }
        }

        const size_t element_id = elem_ids[element];
        Liters<mris6, ComparatorPolicy::Distriburion>::apply(rpds, distance, liters);
        //Liters<mris6, ComparatorPolicy::Distance>::apply(rpds, distance, liters);

        std::cout << element_id << ' ';
        for (auto x : liters)
            std::cout << x << ' ';

        std::cout << std::endl;

        for (size_t i = 0; i < mris6::node_count; ++i)
          global_liters[nodes[i]] = liters[i];
            

      }

    }
  }



}

}  // namespace pkrv