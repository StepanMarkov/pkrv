// Copyright(c) 2026 Stepan Markov - <markov92@rambler.ru>

#ifndef PKRV_MEMORY_MAPPED_H_
#define PKRV_MEMORY_MAPPED_H_

#include <string_view>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace pkrv {

class MemoryMapped {
public:
  MemoryMapped() = default;
  ~MemoryMapped() { close(); }
  MemoryMapped(const MemoryMapped&) = delete;
  MemoryMapped& operator=(const MemoryMapped&) = delete;
  MemoryMapped(MemoryMapped&& other) noexcept = delete;
  MemoryMapped& operator=(MemoryMapped&& other) noexcept = delete;
 
  bool open(const char* path);
  void close();
  
  [[nodiscard]] std::string_view string_view() const noexcept {
    return { static_cast<const char*>(data_), size_ };
  }

private:
  void* data_ = nullptr;
  size_t size_ = 0;
  #ifdef _WIN32
  HANDLE file_handle_ = INVALID_HANDLE_VALUE;
  HANDLE map_handle_ = nullptr;
  #endif
};


}  //namespace pkrv

#endif  // PKRV_MEMORY_MAPPED_H_