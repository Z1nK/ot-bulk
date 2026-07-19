add_executable(unit_tests
  command_parser_test.cpp
  executor_test.cpp
  time_utils_test.cpp
)

target_compile_features(unit_tests PRIVATE cxx_std_20)

target_link_libraries(unit_tests PRIVATE
  comexec::parser-lib-static
  comexec::executor-lib-static
  common::time-utils
  GTest::gtest_main
)

if (MSVC)
  target_compile_options(unit_tests PRIVATE /W4)
else()
  target_compile_options(unit_tests PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

include(GoogleTest)
gtest_discover_tests(unit_tests)
