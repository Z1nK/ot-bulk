#include <command-parser/command_parser.hpp>

#include <gtest/gtest.h>

namespace {

Command makeCommand(std::string name) {
  return Command(std::move(name));
}

std::vector<std::string> names(const std::vector<Command>& commands) {
  std::vector<std::string> result;
  result.reserve(commands.size());
  for (const auto& command : commands) {
    result.push_back(command.name);
  }
  return result;
}

}  // namespace

TEST(CommandParserTest, StaticBlockCompletesAtDefaultSize) {
  CommandParser parser(3);

  EXPECT_FALSE(parser.feedLine(makeCommand("cmd1")));
  EXPECT_FALSE(parser.feedLine(makeCommand("cmd2")));

  auto block = parser.feedLine(makeCommand("cmd3"));
  ASSERT_TRUE(block);
  EXPECT_EQ(names(*block), (std::vector<std::string>{"cmd1", "cmd2", "cmd3"}));
}

TEST(CommandParserTest, FlushReturnsTrailingPartialBlock) {
  CommandParser parser(3);

  EXPECT_FALSE(parser.feedLine(makeCommand("cmd1")));
  EXPECT_FALSE(parser.feedLine(makeCommand("cmd2")));

  auto block = parser.flush();
  ASSERT_TRUE(block);
  EXPECT_EQ(names(*block), (std::vector<std::string>{"cmd1", "cmd2"}));

  // Nothing left to flush after the first flush drained the buffer.
  EXPECT_FALSE(parser.flush());
}

TEST(CommandParserTest, CustomBlockCollectsUntilClosingBrace) {
  CommandParser parser(3);

  EXPECT_FALSE(parser.feedLine(makeCommand("{")));
  EXPECT_FALSE(parser.feedLine(makeCommand("cmd1")));
  EXPECT_FALSE(parser.feedLine(makeCommand("cmd2")));

  auto block = parser.feedLine(makeCommand("}"));
  ASSERT_TRUE(block);
  EXPECT_EQ(names(*block), (std::vector<std::string>{"cmd1", "cmd2"}));
}

TEST(CommandParserTest, OpeningBraceFlushesPendingStaticBlock) {
  CommandParser parser(3);

  EXPECT_FALSE(parser.feedLine(makeCommand("cmd1")));

  auto flushed = parser.feedLine(makeCommand("{"));
  ASSERT_TRUE(flushed);
  EXPECT_EQ(names(*flushed), (std::vector<std::string>{"cmd1"}));

  EXPECT_FALSE(parser.feedLine(makeCommand("cmd2")));
  auto block = parser.feedLine(makeCommand("}"));
  ASSERT_TRUE(block);
  EXPECT_EQ(names(*block), (std::vector<std::string>{"cmd2"}));
}

TEST(CommandParserTest, NestedBracesAreNotEmittedAsCommandsAndOnlyOuterCloseCompletesBlock) {
  CommandParser parser(3);

  EXPECT_FALSE(parser.feedLine(makeCommand("{")));
  EXPECT_FALSE(parser.feedLine(makeCommand("cmd1")));
  EXPECT_FALSE(parser.feedLine(makeCommand("{")));
  EXPECT_FALSE(parser.feedLine(makeCommand("cmd2")));
  EXPECT_FALSE(parser.feedLine(makeCommand("}")));

  auto block = parser.feedLine(makeCommand("}"));
  ASSERT_TRUE(block);
  EXPECT_EQ(names(*block), (std::vector<std::string>{"cmd1", "cmd2"}));
}

TEST(CommandParserTest, StrayClosingBraceOutsideBlockIsIgnored) {
  CommandParser parser(3);

  EXPECT_FALSE(parser.feedLine(makeCommand("}")));
  EXPECT_FALSE(parser.feedLine(makeCommand("cmd1")));
  EXPECT_FALSE(parser.feedLine(makeCommand("cmd2")));

  auto block = parser.feedLine(makeCommand("cmd3"));
  ASSERT_TRUE(block);
  EXPECT_EQ(names(*block), (std::vector<std::string>{"cmd1", "cmd2", "cmd3"}));
}
