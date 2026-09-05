add_library(concurrency INTERFACE)

add_library(common::concurrency ALIAS concurrency)

target_include_directories(concurrency INTERFACE
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/..>
  )
