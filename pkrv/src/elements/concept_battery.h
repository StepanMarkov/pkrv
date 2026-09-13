// Copyright(c) 2026 Stepan Markov - <markov92@rambler.ru>

#ifndef PKRV_CONCEPT_BATTERY_H_
#define PKRV_CONCEPT_BATTERY_H_

#include <concepts>
#include <array>
#include <limits>
#include <ranges>
#include <algorithm>
#include <numeric>

#include "mrls_battery/mrls6.h"

namespace pkrv {

template <typename T>
concept BatteryElement = requires {
  { T::node_count    } -> std::convertible_to<std::size_t>;
  { T::liters_count  } -> std::convertible_to<std::size_t>;
  { T::variant_count } -> std::convertible_to<std::size_t>;
};

enum class ComparatorPolicy {
  Distriburion,
  Distance
};


template<BatteryElement T, ComparatorPolicy type>
struct Liters;

template<BatteryElement T, ComparatorPolicy type>
int comparator(
  const std::array<std::array<double, mris6::node_count>, mris6::node_count>& distance,
  const std::array<size_t, T::node_count>& variants,
  const std::array<size_t, T::node_count>& liters1,
  const std::array<size_t, T::node_count>& liters2) {

  if constexpr (type == ComparatorPolicy::Distriburion) {

    std::array<size_t, (T::liters_count + 1)* T::variant_count> load1 = {}, load2 = {};
    
    for (size_t i = 0; i < T::node_count; ++i) {
      ++load1[variants[i] * (T::liters_count + 1) + liters1[i]];
      ++load2[variants[i] * (T::liters_count + 1) + liters2[i]];
    }
    
    auto max_load1 = std::max_element(load1.begin(), load1.end());
    auto max_load2 = std::max_element(load2.begin(), load2.end());
    
    while ((*max_load1 == *max_load2) && (*max_load1 != 0)) {
      --(*max_load1);
      --*(max_load2);
      max_load1 = std::max_element(load1.begin(), load1.end());
      max_load2 = std::max_element(load2.begin(), load2.end());
    }
    
    if (*max_load1 < *max_load2) return -1;
    if (*max_load1 > *max_load2) return  1;

    return comparator<T, ComparatorPolicy::Distance>(distance, variants, liters1, liters2);
   
  }

  if constexpr (type == ComparatorPolicy::Distance) {
    double load_metric1 = 0.0;
    double load_metric2 = 0.0;
    for (size_t i = 0; i < T::node_count; ++i) {
      const size_t variant_bucket_i = variants[i] * (T::liters_count + 1);
      const size_t load1_bucket_i   = variant_bucket_i + liters1[i];
      const size_t load2_bucket_i   = variant_bucket_i + liters2[i];
      const auto& distance_i = distance[i];
      for (size_t j = i + 1; j < T::node_count; ++j) {
        const size_t variant_bucket_j = variants[j] * (T::liters_count + 1);
        const size_t load1_bucket_j   = variant_bucket_j + liters1[j];
        const size_t load2_bucket_j   = variant_bucket_j + liters2[j];
        const double add_load = 1.0 / (1.0 + distance_i[j]);
        if (load1_bucket_i == load1_bucket_j) load_metric1 += add_load;
        if (load2_bucket_i == load2_bucket_j) load_metric2 += add_load;
      }
    }

    if (load_metric1 < load_metric2) return -1;
    if (load_metric1 > load_metric2) return  1;

  }

  return 0;

}

template<ComparatorPolicy C>
struct Liters<mris6, C> {

  typedef mris6 T;

  static void apply(
    const std::array<size_t, T::node_count>& variants,
    const std::array<std::array<double, T::node_count>, T::node_count>& distance,
    std::array<size_t, T::node_count>& liters) {
    
    liters = {};
    
    //1.
    {
      std::array<size_t, T::variant_count> variant_counters = {};
      for (size_t node = 0; node < T::node_count; ++node)
        ++variant_counters[variants[node] % T::variant_count];
      
      bool is_linear_distribution = true;
    
      for (const size_t& counter : variant_counters)
        is_linear_distribution &= (counter == T::liters_count);
    
      if (is_linear_distribution) {
        for (size_t node = 0; node < T::node_count; ++node)
         liters[node] = (node % T::liters_count) + 1;
       
        return;
    
      }
    }
    
    size_t comporator_counter = 0;
    
    //2.
    {
    
      // а)
    
      size_t node1 = 0;
      size_t node2 = 0; 
      {
        double current_min = std::numeric_limits<double>::max();
        for (size_t i = 0; i < T::node_count; ++i) {
          const auto& distance_i = distance[i];
          for (size_t j = i + 1; j < T::node_count; ++j) {
            if (distance_i[j] < current_min) {
              current_min = distance_i[j];
              node1 = i;
              node2 = j;
            }
          }
        }
      }
    
      // б)
      for (const size_t& var1 : { node1, node2 }) {
    
        // б)
        size_t node3 = 0;
        {
          auto tmp = distance[var1];
          tmp[node1] = std::numeric_limits<double>::max();
          tmp[node2] = std::numeric_limits<double>::max();
          auto iter = std::min_element(tmp.begin(), tmp.end());
          node3 = std::distance(tmp.begin(), iter);
        }
        
        
    
        // в)
        std::array<size_t, T::node_count> liters1 = {};
        for (auto [idx, node] : std::array{ node1, node2, node3 } | std::views::enumerate)
          liters1[node] = (idx % T::liters_count) + 1;
      
      
        // г)
        for (const size_t var2 : { node1, node2, node3 }) {
        
          std::array<size_t, T::node_count> liters2 = liters1;
          size_t node4 = 0;
          {
            auto tmp = distance[var2];
            tmp[node1] = std::numeric_limits<double>::lowest();
            tmp[node2] = std::numeric_limits<double>::lowest();
            tmp[node3] = std::numeric_limits<double>::lowest();
            auto iter = std::max_element(tmp.begin(), tmp.end());
            node4 = std::distance(tmp.begin(), iter);
            liters2[node4] = liters2[var2];
          }
          
          // д)
          for (const size_t var3 : { node1, node2 }) {
          
            std::array<size_t, T::node_count> liters3 = liters2;

            // д)
            size_t node5 = 0;
            {
              auto tmp = distance[var3];
              tmp[node1] = std::numeric_limits<double>::lowest();
              tmp[node2] = std::numeric_limits<double>::lowest();
              tmp[node3] = std::numeric_limits<double>::lowest();
              tmp[node4] = std::numeric_limits<double>::lowest();
              auto iter = std::max_element(tmp.begin(), tmp.end());
              node5 = std::distance(tmp.begin(), iter);
              liters3[node5] = liters3[var3];              
            }
            
            // e) 
            {
              std::array<double, T::liters_count + 1> min_distancies = {};
              min_distancies.fill(std::numeric_limits<double>::max());
            
              for (size_t i = 0; i < mris6::node_count; ++i) {
                if (liters3[i] == 0) {
                  const auto& distance_i = distance[i];
                  for (size_t j = 0; j < T::node_count; ++j) {
                    double& mindist = min_distancies[liters3[j]];
                    mindist = std::min(mindist, distance_i[j]);
                  }
                  auto iter = std::max_element(min_distancies.begin(), min_distancies.end());
                  liters3[i] = std::distance(min_distancies.begin(), iter);
                  break;
                }
              }
            }

            if ((comporator_counter == 0) || (comparator<T,C>(distance, variants, liters, liters3) > 0)) {
              ++comporator_counter;
              liters = liters3;
            }
            
          }                                                                 // (д)
        }                                                                   // (г)    
      }                                                                     // (б)
    }
  }
};


} // namespace pkrv

#endif  // PKRV_CONCEPT_BATTERY_H_