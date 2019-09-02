#include "bluejay/core/pybind11.h"
#include <pybind11/embed.h>
#include <pybind11/eigen.h>

namespace bluejay {

  namespace integral { void init(py::module); }
  namespace molecule { void init(py::module); }
  namespace basis    { void init(py::module); }

}

#include "bluejay/chem/atom.h"

PYBIND11_EMBEDDED_MODULE(bluejay_chem, m) {

  using namespace bluejay;

  py::class_<Atom>(m, "Atom")
    .def_readonly("Z", &Atom::Z)
    .def_readonly("r", &Atom::center)
    ;

  bluejay::basis::init(m.def_submodule("basis"));
  bluejay::molecule::init(m.def_submodule("molecule"));
  bluejay::integral::init(m.def_submodule("integral"));

}
