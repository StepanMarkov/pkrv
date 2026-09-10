// Copyright(c) 2026 Stepan Markov - <markov92@rambler.ru>

#ifndef PKRV_RESULTS_H_
#define PKRV_RESULTS_H_

#include<vector>

namespace pkrv {

namespace State {
  enum DOF {
    LITERS_DISTRIBURION = 0,
    COUNT
  };
}

class Result {
public:
  inline const auto& get(State::DOF result_name) const { return  state_fields_[result_name]; }
  inline auto& get(State::DOF result_name) { return  state_fields_[result_name]; }
private:
  std::vector<int>    state_fields_[State::COUNT] = {};
};


}  //namespace pkrv

#endif  // PKRV_RESULTS_H_