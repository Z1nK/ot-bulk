set(LOCAL_LIB_NAME async-data-sink)

add_library(${LOCAL_LIB_NAME}
  async_data_sink.cpp
  async_data_sink.hpp
)

add_library(datasink::async-data-sink ALIAS ${LOCAL_LIB_NAME})

set_property(TARGET ${LOCAL_LIB_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)

target_compile_features(${LOCAL_LIB_NAME} PUBLIC cxx_std_20)

target_include_directories(${LOCAL_LIB_NAME} PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../..>
  )

find_package(Threads REQUIRED)

target_link_libraries(${LOCAL_LIB_NAME} PUBLIC
  common::idatasink
  common::concurrency
  Threads::Threads
)