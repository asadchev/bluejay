#include "bluejay/core/pybind11.h"
#include <pybind11/stl.h>

#include "bluejay/chem/integral/engine.h"
#include "bluejay/libint2/libint2.h"
#include <libint2/config.h>

namespace bluejay {
namespace libint2 {

  PYBIND11_MODULE(libint2, m) {
    m.def("initialize", bluejay::libint2::initialize);
    m.def(
      "integral_engine",
      [](size_t max_nprim, int max_l) {
        std::shared_ptr<bluejay::IntegralEngine> engine(new LibintEngine(max_nprim, max_l));
        return engine;
      },
      py::arg("max_nprim") = 10,
      py::arg("max_l") = LIBINT_MAX_AM
    );
  }

}
}
