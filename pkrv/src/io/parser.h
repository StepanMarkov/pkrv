// Copyright(c) 2026 Stepan Markov - <markov92@rambler.ru>

#ifndef PKRV_PARSER_H_
#define PKRV_PARSER_H_

#include <memory>
#include <model/model.h>
#include <utils/memory_mapped.h>

namespace pkrv {
	
class Parser {

public:
 
  static std::unique_ptr<Model> Read(const std::string& filepath);

private:
  
  static std::unique_ptr<Model> Read(MemoryMapped& file);
  static std::string ExtractAttribute(std::string_view header, std::string_view key);

};


}  //namespace pkrv

#endif  // PKRV_PARSER_H_