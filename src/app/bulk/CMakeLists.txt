add_executable(bulk bulk.cpp)

target_compile_features(bulk PUBLIC cxx_std_20)

find_package(Threads REQUIRED)

target_link_libraries(bulk PRIVATE
  comexec::executor-lib-shared
    comexec::parser-lib-shared
    common::time-utils
    Threads::Threads
)

if (MSVC)
    target_compile_options(bulk PRIVATE /W4)
else()
    target_compile_options(bulk PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

copy_tsv_files(${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# Installation

install(TARGETS bulk RUNTIME DESTINATION bin)