#include "bluejay/core/pybind11.h"
#include <pybind11/embed.h>
#include <boost/config.hpp>
#include <iostream>

namespace bluejay {
namespace eigen {
  void init(py::module m);
}
}

PYBIND11_EMBEDDED_MODULE(bluejay_core, m) {
  std::cout << "Initializing bluejay_core" << std::endl;
  bluejay::eigen::init(m);
}
