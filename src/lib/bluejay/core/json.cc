#include "bluejay/core/json.h"

namespace bluejay {
namespace json {

  JsonDocument Parse(const std::string &s) {
    JsonDocument d;
    d.Parse(s.c_str());
    return d;
  }

}
}
