set(CONFIG_FILES
  config/config.cc
  config/config.h
)

source_group("config" FILES ${CONFIG_FILES})

list(APPEND LIB_FILES ${CONFIG_FILES})

set(ABSOLUTE_LIB_FILES "")

foreach(FILE_PATH IN LISTS LIB_FILES)
  if(NOT IS_ABSOLUTE "${FILE_PATH}")
    set(FILE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${FILE_PATH}")
  endif()
  list(APPEND ABSOLUTE_LIB_FILES "${FILE_PATH}")
endforeach()

check_files(ABSOLUTE_LIB_FILES)

add_library(pkrv_lib ${LIB_FILES})

add_executable (pkrv "pkrv.cc" )
target_link_libraries(pkrv PRIVATE pkrv_lib)