// Copyright(c) 2026 Stepan Markov - <markov92@rambler.ru>

#ifndef PKRV_SOLVER_H_
#define PKRV_SOLVER_H_

#include <results/results.h>
#include <model/model.h>

namespace pkrv {

class Solver {

public:

Solver(const Model& model, Result& result) :
  model_(model), result_(result) {
};

void Solve();

private:
  const Model& model_;
  Result& result_;

};

} // namespace pkrv

#endif  // PKRV_SOLVER_H_