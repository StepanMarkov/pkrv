// Copyright(c) 2026 Stepan Markov - <markov92@rambler.ru>

#include "parser.h"
#include <ranges>
#include <string_view>
#include <algorithm>
#include <utils/simd_parser.h>

namespace pkrv {

std::unique_ptr<Model> Parser::Read(const std::string& filepath) {

  if (filepath.empty())
    return nullptr;
  
  MemoryMapped memory_mapped;
  if (!memory_mapped.open(filepath.c_str()))
    return nullptr;
  
  return Parser::Read(memory_mapped);

}

std::unique_ptr<Model> Parser::Read(MemoryMapped& file) {

  auto model = std::make_unique<Model>();
  
  std::string_view content = file.string_view();
  const char* pos = content.data();
  const char* const end = pos + content.size();
  const char* line_end = nullptr;
  
  auto CollectLines = [&](std::vector<const char*>& lines) {
    while (pos < end) {
      while (pos < end && (unsigned char)*pos <= 32) ++pos;
      if (pos >= end) break;
      if (*pos != '*') lines.push_back(pos);
      else if ((pos + 1 >= end) || (*(pos + 1) != '*')) break;
      line_end = static_cast<const char*>(memchr(pos, '\n', end - pos));
      pos = line_end ? line_end + 1 : end;
    }
  };

  while (pos < end) {
    pos = static_cast<const char*>(memchr(pos, '*', end - pos));
    if ((pos == nullptr) || (pos >= end)) break;
    line_end = static_cast<const char*>(memchr(pos, '\n', end - pos));
    if ((line_end == nullptr) || (line_end >= end)) break;

    std::string header(pos, line_end - pos);
    std::ranges::transform(header, header.begin(), ::toupper);
    std::string_view header_view{ header };

    pos = line_end + 1;

    if (header.starts_with("*NODE")) {
      std::vector<const char*> lines;
      lines.reserve(content.size() / 60);
      CollectLines(lines);
      
      const size_t node_count = lines.size();
      
      std::vector<size_t> ids(node_count);
      std::vector<double> posX(node_count), posY(node_count), posZ(node_count);
      
      size_t* const ptr_i = ids.data();
      double* const ptr_x = posX.data();
      double* const ptr_y = posY.data();
      double* const ptr_z = posZ.data();
      
      //#pragma omp parallel for
      for (size_t node = 0; node < node_count; ++node) {
        const char* l_ptr = lines[node];
        parse(l_ptr, end, ptr_i[node]);
        parse(l_ptr, end, ptr_x[node]);
        parse(l_ptr, end, ptr_y[node]);
        parse(l_ptr, end, ptr_z[node]);
      }
      
      std::string node_set_name = ExtractAttribute(header_view, "NSET=");
      model->AddNodes(node_set_name, std::move(ids), std::move(posX), std::move(posY), std::move(posZ));

    }


  }

  return model;

}

std::string Parser::ExtractAttribute(std::string_view header, std::string_view key) {

  std::string result;
  auto pos = header.find(key);
  if (pos != std::string_view::npos) {
    const char* start = header.data() + key.size() + pos;
    const char* end = header.data() + header.size();
    parse(start, end, result);
  }

  return result;
}

} // namespace pkrv
