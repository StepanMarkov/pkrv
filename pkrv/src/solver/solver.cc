// Copyright(c) 2026 Stepan Markov - <markov92@rambler.ru>

#include "solver.h"
#include <elements/concept_battery.h>

namespace pkrv {

void Solver::Solve() {

  auto& liters = this->result_.get(State::LITERS_DISTRIBURION);
  liters.resize(model_.index_to_id().size(), 0);

  std::array<size_t, mris6::node_count> variants;
  std::array<std::array<double, mris6::node_count>, mris6::node_count> distance;
  std::array<size_t, mris6::node_count> liters2;

  Liters<mris6, ComparatorPolicy::Distriburion>::apply(variants, distance, liters2);
  Liters<mris6, ComparatorPolicy::Distance>::apply(variants, distance, liters2);
  //applyLiters<mris6, ComparatorPolicy::Distriburion>()

}

}  // namespace pkrv