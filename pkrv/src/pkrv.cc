// Copyright(c) 2026 Stepan Markov - <markov92@rambler.ru>

#include "config/config.h"
#include "io/parser.h"
#include "solver/solver.h"

int main(int argc, char* argv[]) {

  pkrv::Config config;
  pkrv::Result result;
  
  if (!config.ParseArgs(argc, argv))
    return 1;

  auto model = pkrv::Parser::Read(config.inp_path());

  if (model.get()) {
    pkrv::Solver solver(*model, result);
    solver.Solve();
  }

  return 0;
}
