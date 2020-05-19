#include <cstdio>
#include <cstdlib>

#include "error/program_error_codes.hpp"
#include "utils/arguments_parser.hpp"
#include "utils/strings.hpp"
#include "utils/string.hpp"
#include "pe_translator/pe_support.hpp"

using shush::error::ProgramErrorCode;
using namespace shush::args_parser;

int main(int argc, const char** argv) {
  Argument args[] = {
    Argument("file_in",  ArgType::ARG_POSITIONAL),
    Argument("asm",      ArgType::ARG_FLAG      ),
    Argument("o",        ArgType::ARG_OPTIONAL  )
  };
  const size_t args_size = sizeof(args) / sizeof(Argument);
  Argument* in_file_arg = args + 0;
  Argument* asm_arg     = args + 1;
  Argument* out_arg     = args + 2;

  auto* res = ParseArgv(argv, args, args_size);
  if (!res || !args[0].found) {
    fprintf(stderr, shush::strings::ARGS_PARSING_ERROR);
    shush::strings::PrintUsage(argv);
    return static_cast<int>(ProgramErrorCode::INVALID_ARGUMENTS_FORMAT);
  }

  const char* in_file_path  = in_file_arg->value;
  shush::String out_file_path;
  if (out_arg->found) {
    out_file_path.Load(out_arg->value);
  } else {
    out_file_path.Load(in_file_path);
  }
  shush::trans::SetExeExtension(out_file_path);

  FILE* file_in  = nullptr;
  fopen_s(&file_in,  in_file_path, "r");
  FILE* file_out = nullptr;
  fopen_s(&file_out, out_file_path.AsCharArray(), "w");

  return 0;
}