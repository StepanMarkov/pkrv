// Copyright(c) 2026 Stepan Markov - <markov92@rambler.ru>

#include "config/config.h"
#include "io/parser.h"

int main(int argc, char* argv[]) {

  pkrv::Config config;
  
  if (!config.ParseArgs(argc, argv))
    return 1;

  pkrv::Parser parser;
  auto model = parser.Read(config.inp_path());


  return 0;
}
