#include "bluejay/core//pybind11.h"
#include "bluejay/core/python/json.h"
#include <iostream>
#include <boost/config.hpp>

namespace bluejay {
namespace json {

  BOOST_SYMBOL_EXPORT std::string dumps(const py::object &o) {
    return py::cast<std::string>(
      py::module::import("json").attr("dumps")(o)
    );
  }

  BOOST_SYMBOL_EXPORT py::object loads(const std::string &s) {
    return py::module::import("json").attr("loads")(s);
  }


}
}
