#ifndef BLUEJAY_CORE_EXCEPTION_H
#define BLUEJAY_CORE_EXCEPTION_H

#include <exception>
#include <cstdio>
#include <iostream>


namespace bluejay {

  struct Exception : std::exception {
    Exception(const char *what = "") : Exception("%s", what) {}
    template<class ... Args>
    Exception(const char *fmt, const Args& ... args) {
      std::snprintf(what_, sizeof(what_), fmt, cast(args)...);
    }
    const char* what() const noexcept {
      return what_;
    }
  private:
    char what_[128];
  private:
    static const char* cast(const std::string &s) {
      return s.c_str();
    }
    template<typename T>
    static const T& cast(const T &v) {
      static_assert(
        std::is_fundamental< typename std::remove_pointer<T>::type >::value,
        "Exception format expects fundamental type or std::string"
      );
      return v;
    }
  };

  // template<class ... Args>
  // std::invalid_argument invalid_argument(const char *fmt, const Args& ... args) {
  //   Exception e(fmt, args...);
  //   std::cout << e.what() << std::endl;
  //   return std::invalid_argument(e.what());
  // }

  template<class ... Args>
  void Assert(bool expr, const char * msg, const Args& ... args) {
    if (!expr) throw Exception(msg, args...);
  }


}

#endif // BLUEJAY_CORE_EXCEPTION_H
