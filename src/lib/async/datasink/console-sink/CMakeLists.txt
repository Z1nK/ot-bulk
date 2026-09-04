set(LOCAL_LIB_NAME console-sink)

add_library(${LOCAL_LIB_NAME}
  console_data_sink.cpp
  console_data_sink.hpp
)

add_library(datasink::console-sink ALIAS ${LOCAL_LIB_NAME})

target_include_directories(${LOCAL_LIB_NAME} PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../..>
  )

target_link_libraries(${LOCAL_LIB_NAME} PUBLIC
  common::idatasink
)