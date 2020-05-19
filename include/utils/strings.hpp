#ifndef STRINGS_HPP
#define STRINGS_HPP

#include <cstdio>

namespace shush::strings {

const char ARGS_PARSING_ERROR[] = "An error occured while parsing arguments.\n";

const char USAGE_STRING[] = "Usage: %s [-asm|-o <out_filename>] <in_filename>\n";

inline void PrintUsage(const char** argv) {
  fprintf(stderr, USAGE_STRING, argv[0]);
}

}

#endif // CONSTANTS_HPP
