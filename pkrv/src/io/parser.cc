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
    if ((line_end == nullptr) || (line_end >= end))
      line_end = end;

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

    if (header_view.starts_with("*ELEMENT")) {
      std::vector<const char*> lines;
      lines.reserve(content.size() / 60);
      CollectLines(lines);
      
      const std::string current_type = ExtractAttribute(header, "TYPE=");
      const std::string block_name   = ExtractAttribute(header, "ELSET=");
      
      const size_t node_count = 6;
      const size_t element_count = lines.size();

      Model::ElementBlock block;
      block.element_ids.resize(element_count);
      block.connectivity.resize(element_count * node_count);
      block.type_name = current_type;

      size_t* const ptr_ids = block.element_ids.data();
      size_t* const ptr_con = block.connectivity.data();

      //#pragma omp parallel for
      for (size_t element = 0; element < element_count; ++element) {
        const char* l_ptr = lines[element];
        size_t* const cur_ptr_con = ptr_con + element * node_count;
        size_t node_id = -1;
        parse(l_ptr, end, ptr_ids[element]);
        
        for (size_t node = 0; node < node_count; ++node) {
          parse(l_ptr, end, node_id);
          cur_ptr_con[node] = model->id_to_index().get(node_id);
        }
      }

      model->AddBlock(block_name, std::move(block));

    }

    if (header_view.starts_with("*ARMOR_SECTION")) {
      const std::string node_set_name = ExtractAttribute(header_view, "NSET=");
      const std::string armor_name    = ExtractAttribute(header_view, "TYPE=");
      model->AddArmor(node_set_name, armor_name);
    }

    if (header_view.starts_with("*TASK_SECTION")) {
      const std::string element_set_name = ExtractAttribute(header_view, "ELSET=");
      const std::string task_name        = ExtractAttribute(header_view, "TYPE=");
      model->AddTask({ element_set_name, task_name });
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
