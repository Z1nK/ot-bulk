add_executable(bulk bulk.cpp)

target_compile_features(bulk PUBLIC cxx_std_20)

find_package(Threads REQUIRED)

target_link_libraries(bulk PRIVATE
  async::async-lib-shared
    Threads::Threads
)

# libasync.so is installed under lib/async instead of directly under lib, so
# bulk needs an explicit runtime search path to find it after installation.
set_target_properties(bulk PROPERTIES
  INSTALL_RPATH "$ORIGIN/../${CMAKE_INSTALL_LIBDIR}/async"
)

if (MSVC)
    target_compile_options(bulk PRIVATE /W4)
else()
    target_compile_options(bulk PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

copy_tsv_files(${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# Installation

install(TARGETS bulk RUNTIME DESTINATION bin)