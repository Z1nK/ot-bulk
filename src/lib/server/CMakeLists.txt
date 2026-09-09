set(LOCAL_LIB_NAME server)

add_library(${LOCAL_LIB_NAME}
  server.cpp  
)

add_library(server::server ALIAS ${LOCAL_LIB_NAME})

set_property(TARGET ${LOCAL_LIB_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)

target_compile_features(${LOCAL_LIB_NAME} PUBLIC cxx_std_20)

target_include_directories(${LOCAL_LIB_NAME} PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../..>
  )

find_package(Threads REQUIRED)

target_link_libraries(${LOCAL_LIB_NAME} PUBLIC
  Threads::Threads
  Boost::boost
)