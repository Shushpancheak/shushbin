#ifndef ARGUMENTS_PARSER_HPP
#define ARGUMENTS_PARSER_HPP
#include <stdexcept>

namespace shush::args_parser {

enum class ArgType {
  ARG_OPTIONAL,
  ARG_POSITIONAL,
  ARG_FLAG
};

struct Argument {
  Argument(const char* name, const ArgType arg_type)
    : name(name)
    , value(nullptr)
    , arg_type(arg_type)
    , found(false) {
    if (!name && (arg_type == ArgType::ARG_OPTIONAL || arg_type == ArgType::ARG_FLAG)) {
      throw std::runtime_error("Optional argument was given with no name.");
    }
  }

  const char* name;
  const char* value;
  const ArgType arg_type;
  bool found;
};

/**
 * Resets found values in Argument array.
 */
inline void ResetArguments(Argument* res_args, const size_t res_args_size) {
  const Argument* res_args_end = res_args + res_args_size;
  for (Argument* res_arg = res_args; res_arg != res_args_end; ++res_arg) {
    res_arg->found = false;
  }

  return;
}

/**
 * Parses argv searching for arguments specified by result_args, to which result
 * values will be written to.
 *
 * @return pointer to the erroneous argument, which is either not specified,
 *         or the limit number of positional arguments is exceeded. nullptr if no error.
 */
inline const char** ParseArgv(
    const char** argv,
    Argument* res_args,
    const size_t res_args_size
  ) {
  const Argument* res_args_end = res_args + res_args_size;

  for (const char** arg_ptr = argv; *arg_ptr != nullptr; ++arg_ptr) {
    const char* arg = *arg_ptr;

    bool assigned_to_res = false;

    for (Argument* res_arg = res_args; res_arg != res_args_end && !assigned_to_res; ++res_arg) {
      if (res_arg->found) {
        continue;
      }

      switch (res_arg->arg_type) {
      case ArgType::ARG_POSITIONAL : {
        if (arg[0] == '-') {
          continue;
        }

        assigned_to_res = true; 
        res_arg->found = true;
        res_arg->value = arg;
      } break;

      case ArgType::ARG_FLAG : {
        if (arg[0] != '-') {
          continue;
        }

        if (!strcmp(arg + 1, res_arg->name)) {
          assigned_to_res = true;
          res_arg->found = true;
        }
      } break;

      case ArgType::ARG_OPTIONAL : {
        if (arg[0] != '-') {
          continue;
        }

        if (!strcmp(arg + 1, res_arg->name)) {
          ++arg_ptr;

          if ((*arg_ptr) == nullptr) {
            // TODO make Result<T>, specifying the error.
            return arg_ptr - 1;
          }

          assigned_to_res = true;
          res_arg->found = true;
          res_arg->value = *arg_ptr;
        }
      } break;

      default: return arg_ptr;
      }
    }

    if (!assigned_to_res) {
      return arg_ptr;
    }
  }

  return nullptr;
}

}

#endif // ARGUMENTS_PARSER_HPP
