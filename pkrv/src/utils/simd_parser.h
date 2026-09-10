// Copyright(c) 2026 Stepan Markov - <markov92@rambler.ru>

#ifndef PKRV_SIMD_PARSER_H_
#define PKRV_SIMD_PARSER_H_


#include <string>
#include <charconv>

namespace pkrv {

template<typename T>
inline void parse(const char*& start, const char* end, T& value);

template<>
inline void parse<std::string>(const char*& start, const char* end, std::string& value) {
  while (start < end && (*start == ' ' || *start == ',' || *start == '\r')) ++start;
  const char* name_start = start;
  while (start < end && (unsigned char)*start > 32 && *start != ',') ++start;
  value = std::string(name_start, start - name_start);
  if ((start < end) && (*start == ',')) ++start;
}

template<typename T>
inline void parse(const char*& start, const char* end, T& value) {
  while (start < end && (*start == ' ' || *start == '\r')) ++start;
  auto [ptr, ec] = std::from_chars(start, end, value);
  while (ptr < end && (*ptr == ' ' || *ptr == '\r')) ++ptr;
  if ((ptr < end) && (*ptr == ',')) ++ptr;
  start = ptr;
}

}  //namespace pkrv

#endif  // PKRV_HYBRID_MAP_H_