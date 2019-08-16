#include "bluejay/core/pybind11.h"
#include <pybind11/embed.h>
#include <pybind11/stl.h>

#include "bluejay/chem/molecule.h"

namespace bluejay {
namespace molecule {

  // Atom make_atom(const JsonValue &json) {
  //   std::string s = json.GetArray()[0].GetString();
  // }

  std::vector<Atom> make_atoms(const py::object &atoms) {
    std::vector<Atom> v;
    for (const auto &a : atoms) {
      std::string s;
      double x,y,z;
      try {
        typedef std::tuple<double,double,double> double3;
        auto r = std::tie(x,y,z);
        std::tie(s,r) = py::cast< std::tuple<std::string, double3> >(a);
      }
      catch (const std::exception& e) {
        py::print("Can't convert", py::repr(a), " to (str, [ float, float, float ]) tuple");
        throw e;
      }
      v.emplace_back(s, Vector3<double>(x,y,z));
    }
    return v;
  }

  std::unique_ptr<Molecule> make_molecule(py::object atoms, py::object basis) {
    return std::make_unique<Molecule>(
      make_atoms(py::list(atoms)),
      py::cast<BasisSet>(basis)
    );
  }

  py::slice range(const BasisTuple &t) {
    return py::slice(t.index, t.index + nbf(*t.shell), 1);
  }

  void init(py::module m) {

    py::class_<BasisTuple>(m, "BasisTuple")
      .def_property_readonly("shell", [](const BasisTuple &t) { return t.shell; })
      .def_property_readonly("atom",  [](const BasisTuple &t) { return t.atom; })
      .def_property_readonly("index", [](const BasisTuple &t) { return t.index; })
      .def_property_readonly("range", range)

      .def(
        "__iter__",
        [](const BasisTuple &b) {
          auto t = py::make_tuple(b.shell, b.atom, b.index);
          return py::iter(t);
        }
      )
      ;

    py::class_<MoleculeBasis>(m, "MoleculeBasis")
      .def_property_readonly("nbf", &MoleculeBasis::nbf)
      .def(
        "__iter__",
        [](const MoleculeBasis &b) {
          return py::make_iterator(b.begin(), b.end());
        }
      )
      ;

    auto Molecule = py::class_<bluejay::Molecule>(m, "Molecule")
      .def(py::init(&make_molecule), py::arg("geometry"), py::arg("basis"))
      .def_property_readonly("basis", &bluejay::Molecule::basis)
      ;

  }

}
}
