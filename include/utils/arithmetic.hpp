#ifndef SHUSH_ARITHMETIC_HPP
#define SHUSH_ARITHMETIC_HPP

namespace shush::arithm {

template<typename T>
T Min(const T& a, const T& b) {
  return a < b ? a : b;
}

template<typename T>
T Max(const T& a, const T& b) {
  return a > b ? a : b;
}

}

#endif // SHUSH_ARITHMETIC_HPP
