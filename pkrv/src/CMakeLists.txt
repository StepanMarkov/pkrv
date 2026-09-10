set(CONFIG_FILES
  config/config.cc
  config/config.h
)

set(UTILS_FILES
  utils/memory_mapped.cc
  utils/memory_mapped.h
  utils/hybrid_map.h
  utils/simd_parser.h
)

set(MODEL_FILES
  model/model.cc
  model/model.h
)

set (IO_FILES
  io/parser.cc
  io/parser.h
)

source_group("config" FILES ${CONFIG_FILES})
source_group("utils"  FILES ${UTILS_FILES})
source_group("model"  FILES ${MODEL_FILES})
source_group("io"     FILES ${IO_FILES})

list(APPEND LIB_FILES ${CONFIG_FILES})
list(APPEND LIB_FILES ${UTILS_FILES})
list(APPEND LIB_FILES ${MODEL_FILES})
list(APPEND LIB_FILES ${IO_FILES})

set(ABSOLUTE_LIB_FILES "")

foreach(FILE_PATH IN LISTS LIB_FILES)
  if(NOT IS_ABSOLUTE "${FILE_PATH}")
    set(FILE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${FILE_PATH}")
  endif()
  list(APPEND ABSOLUTE_LIB_FILES "${FILE_PATH}")
endforeach()

check_files(ABSOLUTE_LIB_FILES)

add_library(pkrv_lib ${LIB_FILES})
target_include_directories(pkrv_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable (pkrv "pkrv.cc" )
target_link_libraries(pkrv PRIVATE pkrv_lib)