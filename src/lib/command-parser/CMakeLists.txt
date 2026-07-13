add_library(parser-lib-obj OBJECT)
set_property(TARGET parser-lib-obj PROPERTY POSITION_INDEPENDENT_CODE ON)

target_compile_features(parser-lib-obj PRIVATE cxx_std_20)

add_library(parser-lib-shared SHARED)
add_library(parser-lib-static STATIC)

add_library(comexec::parser-lib-shared ALIAS parser-lib-shared) # alias for build-tree usage
add_library(comexec::parser-lib-static ALIAS parser-lib-static) # alias for build-tree usage

target_sources(parser-lib-obj
  PRIVATE
      ${CMAKE_CURRENT_LIST_DIR}/command_parser.cpp
)

target_include_directories(parser-lib-obj PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/..>    
  $<BUILD_INTERFACE:${OT_BULK_AUTOGEN_DIR}>)

## target_link_libraries(parser-lib-obj
##   PUBLIC
## )

# # --------SHARED---------

set_target_properties(parser-lib-shared PROPERTIES
  VERSION ${PROJ_BUILDVERSION}
  SOVERSION ${PROJ_SOVERSION}
  OUTPUT_NAME parser
)

target_link_libraries(parser-lib-shared
  PRIVATE
    $<BUILD_INTERFACE:parser-lib-obj>
)


target_include_directories(parser-lib-shared PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/..>
  $<BUILD_INTERFACE:${OT_BULK_AUTOGEN_DIR}>)

# # --------STATIC---------
if(MSVC)
  set_target_properties(parser-lib-static PROPERTIES
    OUTPUT_NAME parser-static    
  )
else()
  set_target_properties(parser-lib-static PROPERTIES
    OUTPUT_NAME parser
  )
endif()

target_link_libraries(parser-lib-static
  PRIVATE
    $<BUILD_INTERFACE:parser-lib-obj>
)

target_include_directories(parser-lib-static PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/..>
  $<BUILD_INTERFACE:${OT_BULK_AUTOGEN_DIR}>)


# # -- Installation --

# install(TARGETS parser-lib-shared EXPORT parser-lib-targets LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
# set_target_properties(parser-lib-shared PROPERTIES EXPORT_NAME parser-lib SOVERSION ${PROJ_SOVERSION} VERSION ${PROJ_BUILDVERSION})

# install(TARGETS parser-lib-static EXPORT parser-lib-targets LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
# set_target_properties(parser-lib-static PROPERTIES EXPORT_NAME parser-lib-static SOVERSION ${PROJ_SOVERSION} VERSION ${PROJ_BUILDVERSION})


# install(FILES
#   command_parser.hpp
#   DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/command-parser
# )  

# ----------------------------------------------
