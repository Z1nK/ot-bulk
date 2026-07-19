add_library(executor-lib-obj OBJECT)
set_property(TARGET executor-lib-obj PROPERTY POSITION_INDEPENDENT_CODE ON)

target_compile_features(executor-lib-obj PRIVATE cxx_std_20)

add_library(executor-lib-shared SHARED)
add_library(executor-lib-static STATIC)

add_library(comexec::executor-lib-shared ALIAS executor-lib-shared) # alias for build-tree usage
add_library(comexec::executor-lib-static ALIAS executor-lib-static) # alias for build-tree usage

target_sources(executor-lib-obj
  PRIVATE
      ${CMAKE_CURRENT_LIST_DIR}/executor.cpp
)

target_include_directories(executor-lib-obj PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/..>
  $<BUILD_INTERFACE:${OT_BULK_AUTOGEN_DIR}>)

target_link_libraries(executor-lib-obj
  PUBLIC
    $<BUILD_INTERFACE:parser-lib-obj>
)

# # --------SHARED---------

set_target_properties(executor-lib-shared PROPERTIES
  VERSION ${PROJ_BUILDVERSION}
  SOVERSION ${PROJ_SOVERSION}
  OUTPUT_NAME executor
)

target_link_libraries(executor-lib-shared
  PRIVATE
    $<BUILD_INTERFACE:executor-lib-obj>
)


target_include_directories(executor-lib-shared PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/..>
  $<BUILD_INTERFACE:${OT_BULK_AUTOGEN_DIR}>)

# # --------STATIC---------
if(MSVC)
  set_target_properties(executor-lib-static PROPERTIES
    OUTPUT_NAME executor-static    
  )
else()
  set_target_properties(executor-lib-static PROPERTIES
    OUTPUT_NAME executor
  )
endif()

target_link_libraries(executor-lib-static
  PRIVATE
    $<BUILD_INTERFACE:executor-lib-obj>
)

target_include_directories(executor-lib-static PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/..>
  $<BUILD_INTERFACE:${OT_BULK_AUTOGEN_DIR}>)


# # -- Installation --

# install(TARGETS executor-lib-shared EXPORT executor-lib-targets LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
# set_target_properties(executor-lib-shared PROPERTIES EXPORT_NAME executor-lib SOVERSION ${PROJ_SOVERSION} VERSION ${PROJ_BUILDVERSION})

# install(TARGETS executor-lib-static EXPORT executor-lib-targets LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
# set_target_properties(executor-lib-static PROPERTIES EXPORT_NAME executor-lib-static SOVERSION ${PROJ_SOVERSION} VERSION ${PROJ_BUILDVERSION})


# install(FILES
#   executor.hpp
#   DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/executor
# )  

# ----------------------------------------------

