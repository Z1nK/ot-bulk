add_library(idatasink INTERFACE)

add_library(common::idatasink ALIAS idatasink)

target_include_directories(idatasink INTERFACE
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../..>
  )