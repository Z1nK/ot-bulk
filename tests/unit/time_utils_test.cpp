#include <time-utils/time.hpp>

#include <algorithm>
#include <cctype>

#include <gtest/gtest.h>

TEST(TimeUtilsTest, GetCurrentTimeStrIsNotEmpty) {
  EXPECT_FALSE(getCurrentTimeStr().empty());
}

TEST(TimeUtilsTest, GetUnixTimestampStringIsNumeric) {
  const std::string timestamp = getUnixTimestampString();

  ASSERT_FALSE(timestamp.empty());
  EXPECT_TRUE(std::all_of(timestamp.begin(), timestamp.end(),
                          [](unsigned char c) { return std::isdigit(c) != 0; }));
}
