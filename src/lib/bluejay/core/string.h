#ifndef BLUEJAY_STRING_H
#define BLUEJAY_STRING_H

#include <string>
#include <sstream>

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
      ss << *it;
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

  inline std::string str(const std::string &s) {
    return s;
  }

}

#endif // BLUEJAY_STRING_H
