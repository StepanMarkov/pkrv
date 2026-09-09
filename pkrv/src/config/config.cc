// Copyright(c) 2026 Stepan Markov - <markov92@rambler.ru>

#include "config.h"
#include <stdexcept>

namespace pkrv {

bool Config::ParseArgs(int argc, char* argv[]) {

  if ((argc <= 0) || argv == nullptr) 
    return false;

  bool sucsess = true;
  Config current_config = *this;
  
  try {
  
  	char** arg = argv;
  	char** arg_end = argv + argc;
  	directory_ = *arg++;
  
  	while (sucsess && (arg < arg_end)) {
      std::string head = *arg++;

      if (head[0] != '-')
        throw std::invalid_argument("Argument is invalid, use - or --");

      if (arg < arg_end) {
      	std::string value = *arg++;
      	if ((head == "-i") || (head == "--input"))
      	  inp_path_ = value;
      	else if ((head == "-o") || (head == "--output"))
      	  out_path_ = value;
        else sucsess = false;
      }
      else sucsess = false;
    }

  }
  catch (const std::invalid_argument&) {
  	sucsess = false;
  }
  catch (const std::out_of_range&) {
  	sucsess = false;
  }
  
  if (!sucsess)
    *this = current_config;
  
  return sucsess;

}

} // namespace pkrv