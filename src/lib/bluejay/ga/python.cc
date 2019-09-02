#include "bluejay/ga/ga.h"
#include "bluejay/core/pybind11.h"
#include <pybind11/embed.h>

namespace bluejay {
namespace ga {

  PYBIND11_EMBEDDED_MODULE(bluejay_ga, m) {

    py::class_<GlobalArray, Array, std::shared_ptr<GlobalArray> >(m, "GlobalArray")
      .def(py::init<const std::vector<int64_t>&>())
      ;

  }

}
}
