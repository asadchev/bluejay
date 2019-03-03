#ifndef BLUEJAY_STRING_H
#define BLUEJAY_STRING_H

#include <string>
#include <algorithm>
#include <sstream>
#include <iostream>

namespace bluejay {

  template<typename T>
  inline std::string str(const T &v) {
    std::stringstream ss;
    ss << v;
    return ss.str();
  }

  inline std::string str(const std::string &s) {
    return s;
  }

}

namespace bluejay {
namespace string {

  template<class F>
  inline std::string transform(F f, const std::string &s) {
    std::string t = s;
    std::transform(t.begin(), t.end(), t.begin(), f);
    return t;
  }

  inline std::string uppercase(const std::string &s) {
    return transform([](const auto &c) { return std::toupper(c); }, s);
  }

  inline std::string lowercase(const std::string &s) {
    return transform([](const auto &c) { return std::tolower(c); }, s);
  }

  template<class S = std::string, typename Iterator>
  S join(const std::string &s, Iterator it, Iterator end) {
    std::stringstream ss;
    while (it != end) {
      ss << str(*it);
      ++it;
      if (it != end) ss << ",";
    }
    const auto &str = ss.str();
    return S(str.begin(), str.end());
  }

}
}

namespace bluejay {

  using bluejay::string::join;

}

#endif // BLUEJAY_STRING_H
