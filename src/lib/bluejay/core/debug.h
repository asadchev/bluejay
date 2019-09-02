#ifndef BLUEJAY_DEBUG_H
#define BLUEJAY_DEBUG_H

#include <cstdio>
#include "bluejay/core/string.h"

namespace bluejay {
namespace debug {


  template<typename T>
  T ctype(const T &v) {
    static_assert(
      std::is_fundamental< typename std::remove_pointer<T>::type >::value,
      "Expected fundamental type"
    );
    return v;
  }

  inline const char* ctype(const std::string &s) {
    return s.c_str();
  }


  template<typename T>
  T* printify(const T* &p) {
    return p;
  }

  template<typename T>
  T printify(const T &v, typename std::enable_if< std::is_fundamental<T>::value >::type* = nullptr) {
    return v;
  }

  template<typename T>
  std::string printify(const T &v, typename std::enable_if< !std::is_fundamental<T>::value >::type* = nullptr) {
    return str(v);
  }

  template<typename T, size_t N>
  inline std::string printify(const std::array<T,N> &v) {
    return "[" + string::join(",", v.begin(), v.end()) + "]";
  }

  template<typename T>
  inline std::string printify(const std::vector<T> &v) {
    return "[" + string::join(",", v.begin(), v.end()) + "]";
  }

  template<class ... Args>
  void print(std::string fmt, const Args& ... args) {
    fmt += "\n";
    std::printf(fmt.c_str(), ctype(printify(args))...);
  };

}
}

#endif /* BLUEJAY_DEBUG_H */
