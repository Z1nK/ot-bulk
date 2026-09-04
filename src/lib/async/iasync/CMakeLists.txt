add_library(async-lib-obj OBJECT)
set_property(TARGET async-lib-obj PROPERTY POSITION_INDEPENDENT_CODE ON)

target_compile_features(async-lib-obj PRIVATE cxx_std_20)

add_library(async-lib-shared SHARED)
add_library(async-lib-static STATIC)

add_library(async::async-lib-shared ALIAS async-lib-shared) # alias for build-tree usage
add_library(async::async-lib-static ALIAS async-lib-static) # alias for build-tree usage

target_sources(async-lib-obj
  PRIVATE
      ${CMAKE_CURRENT_LIST_DIR}/iasync.cpp
)

target_include_directories(async-lib-obj PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/..>
  $<BUILD_INTERFACE:${OT_BULK_AUTOGEN_DIR}>)

find_package(Threads REQUIRED)

target_link_libraries(async-lib-obj
  PRIVATE
    comexec::executor-lib-static
    comexec::parser-lib-static
    datasink::console-sink
    datasink::file-sink
    datasink::async-data-sink
    common::idatasink
    Threads::Threads
)

# # --------SHARED---------

set_target_properties(async-lib-shared PROPERTIES
  VERSION ${PROJ_BUILDVERSION}
  SOVERSION ${PROJ_SOVERSION}
  OUTPUT_NAME async
)

target_link_libraries(async-lib-shared
  PRIVATE
    $<BUILD_INTERFACE:async-lib-obj>
)

target_include_directories(async-lib-shared PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/..>
  $<BUILD_INTERFACE:${OT_BULK_AUTOGEN_DIR}>)

# # --------STATIC---------
if(MSVC)
  set_target_properties(async-lib-static PROPERTIES
    OUTPUT_NAME async-static
  )
else()
  set_target_properties(async-lib-static PROPERTIES
    OUTPUT_NAME async
  )
endif()

target_link_libraries(async-lib-static
  PRIVATE
    $<BUILD_INTERFACE:async-lib-obj>
)

target_include_directories(async-lib-static PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/..>
  $<BUILD_INTERFACE:${OT_BULK_AUTOGEN_DIR}>)


# # -- Installation --

install(TARGETS async-lib-shared EXPORT async-lib-targets LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/async)
set_target_properties(async-lib-shared PROPERTIES EXPORT_NAME async-lib SOVERSION ${PROJ_SOVERSION} VERSION ${PROJ_BUILDVERSION})

# install(TARGETS async-lib-static EXPORT async-lib-targets LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
# set_target_properties(async-lib-static PROPERTIES EXPORT_NAME async-lib-static SOVERSION ${PROJ_SOVERSION} VERSION ${PROJ_BUILDVERSION})


# install(FILES
#   iasync.hpp
#   DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/iasync
# )

# ----------------------------------------------
