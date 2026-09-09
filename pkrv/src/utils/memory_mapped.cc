// Copyright(c) 2026 Stepan Markov - <markov92@rambler.ru>

#include "memory_mapped.h"

namespace pkrv {

bool MemoryMapped::open(const char* path) {

  #ifdef _WIN32

  file_handle_ = CreateFileA(path,
    GENERIC_READ, 
    FILE_SHARE_READ, 
    nullptr, 
    OPEN_EXISTING, 
    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
    nullptr);

  if (file_handle_ == INVALID_HANDLE_VALUE) {
    close(); return false;
  }

  LARGE_INTEGER size;

  if (!GetFileSizeEx(file_handle_, &size)) {
    close(); return false;
  }
  
  size_ = static_cast<size_t>(size.QuadPart);
  
  if (size_ > 0) {

    map_handle_ = CreateFileMappingA(file_handle_, 
     nullptr, PAGE_READONLY, 0, 0, nullptr);

    if (!map_handle_) {
      close(); return false;
    }
  
    data_ = MapViewOfFile(map_handle_, 
      FILE_MAP_READ, 0, 0, 0);

    if (!data_) {
      close(); return false;
    }

  }
  #else
  int fd = ::open(path, O_RDONLY);

  if (fd == -1) {
    close(); return false;
  }
  
  struct stat sb;
  if (fstat(fd, &sb) == -1) {
    ::close(fd);
    close(); return false;
  }
  size_ = static_cast<size_t>(sb.st_size);
  
  if (size_ > 0) {

    data_ = mmap(nullptr, size_, 
      PROT_READ, MAP_PRIVATE, fd, 0);

    ::close(fd);
    if (data_ == MAP_FAILED) {
      close(); return false;
    }
  }
  else {
    ::close(fd);
  }
  #endif
  return true;
 }


void MemoryMapped::close() {
  #ifdef _WIN32

  if (data_) 
    UnmapViewOfFile(data_);

  if (map_handle_) 
    CloseHandle(map_handle_);

  if (file_handle_ != INVALID_HANDLE_VALUE) 
    CloseHandle(file_handle_);

  file_handle_ = INVALID_HANDLE_VALUE;
  map_handle_ = nullptr;
  #else
  if (data_ && data_ != MAP_FAILED) munmap(data_, size_);
  #endif
  data_ = nullptr;
  size_ = 0;
}

}  // namespace pkrv