// Copyright(c) 2026 Stepan Markov - <markov92@rambler.ru>

#ifndef PKRV_CONCEPT_BATTERY_H_
#define PKRV_CONCEPT_BATTERY_H_

#include <concepts>
#include <array>
#include <limits>
#include <ranges>
#include "mrls_battery/mrls6.h"

namespace pkrv {

template <typename T>
concept BatteryElement = requires {
  { T::node_count    } -> std::convertible_to<std::size_t>;
  { T::liters_count  } -> std::convertible_to<std::size_t>;
  { T::variant_count } -> std::convertible_to<std::size_t>;
};

template<BatteryElement T>
void applyLiters(
  const std::array<size_t, T::node_count> variants,
  const std::array<std::array<double, T::node_count>, T::node_count>& distance,
  std::array<size_t, T::node_count>& liters);

template<>
void applyLiters<mris6>(
  const std::array<size_t, mris6::node_count> variants,
  const std::array<std::array<double, mris6::node_count>, mris6::node_count>& distance,
  std::array<size_t, mris6::node_count>& liters) {

  liters = {};

  //1.
  {
    std::array<size_t, mris6::variant_count> variant_counters = {};
    for (size_t node = 0; node < mris6::node_count; ++node)
      ++variant_counters[variants[node] % mris6::variant_count];
    
    bool is_linear_distribution = true;

    for (const size_t& counter : variant_counters)
      is_linear_distribution &= (counter == mris6::variant_count);

    if (is_linear_distribution) {
      for (size_t node = 0; node < mris6::node_count; ++node)
       liters[node] = (node % mris6::liters_count) + 1;
     
      return;

    }
  }

  //2.
  {

    // а)
 
    size_t node1 = 0;
    size_t node2 = 0; 
    {
      double current_min = std::numeric_limits<double>::lowest();
      for (size_t i = 0; i < mris6::node_count; ++i) {
        const auto& distance_i = distance[i];
        for (size_t j = i + 1; i < mris6::node_count; ++j) {
          if (distance_i[j] < current_min) {
            current_min = distance_i[j];
            node1 = i;
            node2 = j;
          }
        }
      }
    }



    // б)

    size_t node3 = 0;
    {
      double current_min = std::numeric_limits<double>::max();
      for (size_t i = 0; i < mris6::node_count; ++i) {
        const auto& distance_i = distance[i];
        for (const size_t j : {node1, node2}) {
          if ((i < j) && (distance_i[j] < current_min)) {
            current_min = distance_i[j];
            node3 = i;
          }
        }
      }
    }

    // в)

    {
      for (auto [idx, node] : std::array{ node1, node2, node3 } | std::views::enumerate)
        liters[node] = (idx % mris6::liters_count) + 1;
    }

    // г)

    size_t node4 = 0;
    {
      double current_max = std::numeric_limits<double>::min();
      for (size_t i = 0; i < mris6::node_count; ++i) {
        const auto& distance_i = distance[i];
        for (const size_t j : {node1, node2, node3}) {
          if ((i < j) && (distance_i[j] > current_max)) {
            current_max = distance_i[j];
            liters[i] = liters[j];
            node4 = i;
          }
        }
      }
    }

    // д)

    size_t node5 = 0;
    {
      size_t node_tmp = 0;
      double current_max = std::numeric_limits<double>::min();
      for (size_t i = 0; i < mris6::node_count; ++i) {
        if (liters[i] != 0) continue;
        const auto& distance_i = distance[i];
        for (const size_t j : {node1, node2 }) {
          if (distance_i[j] > current_max) {
            current_max = distance_i[j];
            node5 = i;
            node_tmp = j;
          }
        }
      }

      liters[node5] = liters[node_tmp];

    }

    // e) 
    {
      std::array<double, mris6::liters_count + 1> min_distancies = {};
      min_distancies.fill(std::numeric_limits<double>::max());

      for (size_t i = 0; i < mris6::node_count; ++i) {
        if (liters[i] == 0) {
          const auto& distance_i = distance[i];
          for (size_t j = 0; j < mris6::node_count; ++j) {
            double& mindist = min_distancies[liters[j]];
            mindist = std::min(mindist, distance_i[j]);
          }
          auto iter = std::max_element(min_distancies.begin(), min_distancies.end());
          liters[i] = std::distance(min_distancies.begin(), iter);
          break;
        }
      }
    }
  }
}


} // namespace pkrv

#endif  // PKRV_CONCEPT_BATTERY_H_