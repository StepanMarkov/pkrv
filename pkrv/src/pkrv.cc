// Copyright(c) 2026 Stepan Markov - <markov92@rambler.ru>

#include "config/config.h"

int main(int argc, char* argv[]) {

  pkrv::Config config;
  
  if (!config.ParseArgs(argc, argv)) {
    return 1;
  }

  return 0;
}
