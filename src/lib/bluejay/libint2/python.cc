#include "bluejay/core/pybind11.h"
#include <pybind11/stl.h>

#include "bluejay/chem/integral/engine.h"
#include "bluejay/chem/basis.h"
#include "bluejay/libint2/libint2.h"
#include <libint2/config.h>

namespace bluejay {
namespace libint2 {

  std::shared_ptr<bluejay::IntegralEngine> integral_engine() {
    return std::make_shared<bluejay::libint2::LibintEngine>();
  }

  PYBIND11_MODULE(libint2, m) {
    m.def("initialize", bluejay::libint2::initialize);
    m.def("integral_engine", &integral_engine);
  }

}
}
