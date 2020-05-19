#ifndef SHUSH_PE_SUPPORT_HPP
#define SHUSH_PE_SUPPORT_HPP

#include "utils/string.hpp"

namespace shush::pe_trans {

namespace detail {

const char SHUSHEXE_STR[] = ".shushexe";
const char EXE_STR[]      = ".exe";

}

inline void SetExeExtension(shush::String& string) {
  char* str = string.AsCharArray();
  const size_t str_len = strlen(str);
  const size_t exe_len = strlen(detail::EXE_STR);

  if (str_len < exe_len) {
    string.Resize(exe_len + 1);
    str = string.AsCharArray();
    strcpy_s(str, exe_len + 1, detail::EXE_STR);
  } else {
    strcpy_s(
      str + str_len - strlen(detail::SHUSHEXE_STR),
      exe_len + 1,
      detail::EXE_STR
    );
  }
}

}

#endif // PE_SUPPORT_HPP
