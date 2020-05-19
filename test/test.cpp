#include <iostream>
#include "gtest/gtest.h"
#include "arg_parser_tests.cpp"

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}