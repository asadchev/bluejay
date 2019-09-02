#include "bluejay/core/pybind11.h"
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <boost/variant.hpp>

#include "bluejay/chem/molecule.h"

namespace bluejay {
namespace molecule {

  // Atom make_atom(const JsonValue &json) {
  //   std::string s = json.GetArray()[0].GetString();
  // }

  std::vector<Atom> make_atoms(const py::object &atoms) {
    std::vector<Atom> v;
    typedef boost::variant<std::string,int> Element;
    for (const auto &a : atoms) {
      //debug::print("atom: a);
      Element e;
      double x,y,z;
      try {
        typedef std::tuple<double,double,double> double3;
        auto r = std::tie(x,y,z);
        std::tie(e,r) = py::cast< std::tuple<Element, double3> >(a);
        if (const auto *s = boost::get<std::string>(&e)) {
          v.push_back(Atom(*s, {x,y,z}));
        }
        else {
          int Z = boost::get<int>(e);
          v.push_back(Atom(Z, {x,y,z}));
        }
      }
      catch (const std::exception& e) {
        throw Exception(
          "Can't convert %s to (element, [ float, float, float ]) tuple",
          str(a)
        );
      }
    }
    return v;
  }

  std::unique_ptr<Molecule> make_molecule(py::object atoms, py::object basis) {
    return std::make_unique<Molecule>(
      make_atoms(py::list(atoms)),
      py::cast<BasisSet>(basis)
    );
  }

  void init(py::module m) {

    auto Molecule = py::class_<bluejay::Molecule>(m, "Molecule")
      .def(py::init(&make_molecule), py::arg("geometry"), py::arg("basis"))
      .def_property_readonly("basis", &bluejay::Molecule::basis)
      ;

  }

}
}
