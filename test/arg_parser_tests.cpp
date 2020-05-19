#pragma once

#include "utils/arguments_parser.hpp"
#include "gtest/gtest.h"

using namespace shush::args_parser;

TEST(arg_parser, simple_args) {
  Argument parsed_args[] = {
    Argument("pos_arg_0",  ArgType::ARG_POSITIONAL),
    Argument("pos_arg_1",  ArgType::ARG_POSITIONAL),
    Argument("flag_0",     ArgType::ARG_FLAG),
    Argument("pos_arg_2",  ArgType::ARG_POSITIONAL),
    Argument("flag_1",     ArgType::ARG_FLAG),
    Argument("optional_0", ArgType::ARG_OPTIONAL),
    Argument("optional_1", ArgType::ARG_OPTIONAL),
    Argument("flag_2",     ArgType::ARG_FLAG),
  };
  const size_t parsed_args_size = sizeof(parsed_args) / sizeof(Argument);

  const char* argv_0[] = {
    "val_pos_0",
    "-flag_0",
    "-flag_1",
    "-error",
    "val_pos_1",
    "val_pos_2",
    "-optional_1",
    "val_optional_1",
    nullptr
  };

  auto* res = ParseArgv(argv_0, parsed_args, parsed_args_size);
  ASSERT_EQ(res, argv_0 + 3);

  ResetArguments(parsed_args, parsed_args_size);

  const char* argv_1[] = {
    "val_pos_0",
    "-flag_0",
    "-flag_1",
    "val_pos_1",
    "val_pos_2",
    "-optional_1",
    "val_optional_1",
    nullptr
  };

  res = ParseArgv(argv_1, parsed_args, parsed_args_size);
  ASSERT_EQ(parsed_args[0].value, argv_1[0]);
  ASSERT_EQ(parsed_args[1].value, argv_1[3]);
  ASSERT_EQ(parsed_args[2].found, true);
  ASSERT_EQ(parsed_args[3].value, argv_1[4]);
  ASSERT_EQ(parsed_args[4].found, true);
  ASSERT_EQ(parsed_args[6].value, argv_1[6]);

  ASSERT_FALSE(parsed_args[5].found);
  ASSERT_FALSE(parsed_args[7].found);
}
