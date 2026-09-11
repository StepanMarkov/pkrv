// Copyright(c) 2026 Stepan Markov - <markov92@rambler.ru>

#include "solver.h"

namespace pkrv {

void Solver::Solve() {

  auto& liters = this->result_.get(State::LITERS_DISTRIBURION);
  liters.resize(model_.index_to_id().size(), 0);

}

}  // namespace pkrv