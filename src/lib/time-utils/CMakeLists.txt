add_library(time-utils INTERFACE)

add_library(common::time-utils ALIAS time-utils)

target_include_directories(time-utils INTERFACE
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/..>
  )