#include "bluejay/core//forward.h"
#include "bluejay/core/json.h"
#include <iostream>

namespace bluejay {
namespace json {

  std::string dumps(const py::object&);
  py::object loads(const std::string&);

}
}
