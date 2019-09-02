#ifndef BLUEJAY_JSON_H
#define BLUEJAY_JSON_H

#include <rapidjson/document.h>
#include <vector>

namespace bluejay {
namespace json {

  typedef rapidjson::Document JsonDocument;
  typedef rapidjson::Document::GenericValue JsonValue;

  JsonDocument Parse(const std::string&);

  template<typename T>
  T Get(const rapidjson::Document::GenericValue &v) {
    return v.template Get<T>();
  }

  template<>
  inline std::string Get(const rapidjson::Document::GenericValue &v) {
    return v.GetString();
  }

  template<typename T>
  std::vector<T> GetArray(const rapidjson::Document::GenericValue &v) {
    const auto &array = v.GetArray();
    std::vector<T> u(array.Size());
    for (size_t i = 0; i < u.size(); ++i) {
      u[i] = Get<T>(array[i]);
    }
    return u;
  }

}
}

namespace bluejay {

  using bluejay::json::JsonDocument;
  using bluejay::json::JsonValue;

}

#endif // BLUEJAY_JSON_H
