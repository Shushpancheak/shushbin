#ifndef SHUSH_STRING_HPP
#define SHUSH_STRING_HPP

#include <algorithm>
#include <cassert>
#include <cstring>

#include "arithmetic.hpp"

namespace shush {

class String {
public:
  String()
    : str_(nullptr)
    , loaded_(false)
    , is_static_(false) {}

  ~String() {
    if (!IsLoaded()) {
      return;
    }
    delete[] str_;
  }

  bool IsLoaded() const {
    return loaded_;
  }

  bool IsStatic() const {
    return is_static_;
  }

  /**
   * Works only if String not IsStatic.
   */
  void Resize(const size_t new_size) {
    assert(!is_static_);

    char* new_str = new char[new_size + 1];

    if (IsLoaded()) {
      strcpy_s(new_str, arithm::Min(strlen(str_), new_size) + 1, str_);
      delete[] str_;
      str_ = new_str;
    }

    return;
  }

  /**
   * Copy source_str to the buffer.
   *
   * @return 1 if string is already loaded.
   */
  int Load(const char* source_str) {
    if (IsLoaded()) {
      return 1;
    }

    const size_t str_size = strlen(source_str);
    str_ = new char[str_size + 1];
    strcpy_s(str_, str_size + 1, source_str);

    loaded_ = true;
    is_static_ = false;
    return 0;
  }

  /**
   * Set inner pointer to the given string. Use this if you're sure that
   * lifetime of the string given exceeds that of this String.
   *
   * @return 1 if string is already loaded.
   */
  int PointTo(char* source_str) {
    if (IsLoaded()) {
      return 1;
    }
    str_ = source_str;
    loaded_ = true;
    is_static_ = true;

    return 0;
  }


             String(const String&  other) = delete;
             String(const String&& other) = delete;
  String& operator=(const String&  other) = delete;
  String& operator=(const String&& other) = delete;

  // ReSharper disable once CppMemberFunctionMayBeConst
  char* AsCharArray() {
    return str_;
  }

private:
  char* str_;
  bool  loaded_;
  bool  is_static_;
};

}

#endif // STRING_HPP
