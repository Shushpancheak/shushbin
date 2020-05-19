#ifndef SHUSH_VECTOR_HPP
#define SHUSH_VECTOR_HPP

#include <cassert>
#include <algorithm>

#include "arithmetic.hpp"

namespace shush {

template<typename T>
class Vector {
public:
  explicit Vector(const size_t reserve = 1)
    : buf(new T[reserve])
    , buf_size(arithm::Max(reserve, static_cast<size_t>(1)))
    , obj_cnt(0) {}

  ~Vector() {
    DestroyAllObjects();
    delete[] buf;
  }

  T& operator[](size_t i) {
    assert(i < obj_cnt);
    return buf[i];
  }

  void Resize(size_t new_size) {
    if (new_size == 0) {
      new_size = 1;
    }

    T* new_buf = new T[new_size];

    const size_t new_obj_cnt = arithm::Min(obj_cnt, new_size);
    const size_t new_buf_size = new_size;

    std::copy(buf, buf + new_obj_cnt, new_buf);

    DestroyAllObjects();
    delete[] buf;

    buf      = new_buf;
    obj_cnt  = new_obj_cnt;
    buf_size = new_buf_size;
  }

  void PushBack(const T& obj) {
    if (obj_cnt == buf_size) {
      Resize(obj_cnt * 2);
    }

    new (buf + obj_cnt++) T(obj);
  }

  T PopBack() {
    if (obj_cnt == buf_size / 4) {
      Resize(buf_size / 2);
    }

    return buf[--obj_cnt];
  }

  size_t GetSize() const {
    return obj_cnt;
  }

  size_t GetReserve() const {
    return buf_size;
  }

  void Sort() {
    std::sort(buf, buf + obj_cnt);
  }

private:
  void DestroyAllObjects() {
    for (size_t i = 0; i < obj_cnt; ++i) {
      buf[i].~T();
    }
  }

  T* buf;
  size_t buf_size;
  size_t obj_cnt;
};

}

#endif // VECTOR_HPP
