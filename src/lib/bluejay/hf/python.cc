#include "bluejay/hf/fock.h"
#include "bluejay/core/pybind11.h"
#include <pybind11/eigen.h>

namespace bluejay {
namespace hf {

  PYBIND11_MODULE(bluejay_hf, m) {

    py::class_<FockEngine>(m, "FockEngine")
      .def(py::init< const Basis&, std::shared_ptr<IntegralEngine> >())
      .def("overlap", &FockEngine::overlap)
      .def("kinetic", &FockEngine::kinetic)
      .def("nuclear", &FockEngine::nuclear)
      .def("fock2e", &FockEngine::fock2e)
      .def_static("diagonalizer", &FockEngine::diagonalizer)
      ;

  }

}
}
