// Copyright(c) 2026 Stepan Markov - <markov92@rambler.ru>

#ifndef PKRV_CONFIG_H_
#define PKRV_CONFIG_H_

#include<string>

namespace pkrv {

class Config {

public:
  Config() = default;
  bool ParseArgs(int argc, char* argv[]);
  inline const std::string& inp_path()  const { return inp_path_;  };
  inline const std::string& out_path()  const { return out_path_;  };
  inline const std::string& directory() const { return directory_; };

private:
  std::string inp_path_ = "job.inp";
  std::string out_path_ = "job.out";
  std::string directory_ = "";

};

}  // namespace pkrv

#endif  // PKRV_CONFIG_H_
