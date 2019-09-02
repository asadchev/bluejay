#include "bluejay/core/pybind11.h"
#include "bluejay/core/python/json.h"
#include "bluejay/chem/basis.h"

#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <pybind11/eigen.h>

namespace bluejay {
namespace basis {

  auto make_basis_set_shell(std::string s, const std::vector<BasisSet::Shell::Primitive> &p) {
    return std::make_unique<BasisSet::Shell>(s, p);
  }

  std::shared_ptr<BasisSet> make_basis_set(const std::map<std::string, py::object> basis) {
    std::map< std::string, std::vector< std::shared_ptr<BasisSet::Shell> > > data;
    for (const auto &kv : basis) {
      try {
        std::string e = kv.first;
        auto &shells = data[e];
        for (const auto &s : kv.second) {
          shells.push_back(
            py::cast< std::shared_ptr<BasisSet::Shell> >(s)
          );
        }
      }
      catch (const py::error_already_set &e) {
        std::cout << e.what() << std::endl;
      }
    }
    return std::make_unique<BasisSet>(data);
  }

  inline std::string __repr__(const BasisSet::Shell &s) {
    return s.symbol() + ": " + py::str(py::cast(s.data())).cast<std::string>();
  }

  template<class O, typename K>
  py::object __getitem__(const O &o, const K &k) {
    return py::cast(o[k]);
  }

  template<class O>
  py::object iter(const O &o) {
    return py::make_iterator(o.begin(), o.end());
  }

  std::shared_ptr<BasisSet> parse_basis_set_json(py::iterator it) {
    std::string json;
    for (auto line : it) {
      json += py::cast<std::string>(line);
    }
    auto doc = json::Parse(json);
    return BasisSet::ParseJson(doc);
  }

  py::slice range(const Basis::Shell &s) {
    Range<size_t> r = s;
    return py::slice(r.start, r.stop, 1);
  }

  void init(py::module m) {

    auto PyBasisSet = py::class_<BasisSet, std::shared_ptr<BasisSet> >(m, "BasisSet")
      .def(py::init<std::string, std::string>(), py::arg("name"), py::arg("path") = "")
      .def(py::init(&make_basis_set))
      .def("__getitem__", __getitem__<BasisSet,std::string>)
      .def("__iter__", &iter<BasisSet>)
      ;

    auto PyGaussianShell = py::class_<GaussianShell, std::shared_ptr<GaussianShell> >(m, "GaussianShell")
      .def(py::init(&make_basis_set_shell))
      .def("__repr__", __repr__)
      .def_property_readonly("data", &BasisSet::Shell::data)
      .def_property_readonly("L", &BasisSet::Shell::L)
      .def("__getitem__", __getitem__<BasisSet::Shell,size_t>)
      .def("__iter__", &iter<GaussianShell>)
      ;

    py::implicitly_convertible<py::str, BasisSet>();

    auto PyBasis = py::class_<Basis>(m, "Basis")
      .def_property_readonly("nbf", &Basis::nbf)
      .def_property_readonly("atoms", &Basis::atoms)
      .def(
        "__iter__",
        [](const Basis &b) {
          return py::make_iterator(b.begin(), b.end());
        }
      )
      ;

    py::class_<Basis::Shell>(PyBasis, "Shell")
      .def_property_readonly("shell", [](const Basis::Shell &t) { return t.shell; })
      .def_property_readonly("atom",  [](const Basis::Shell &t) { return t.atom; })
      .def_property_readonly("index", [](const Basis::Shell &t) { return t.index; })
      .def_property_readonly("range", range)

      .def(
        "__iter__",
        [](const Basis::Shell &b) {
          auto t = py::make_tuple(b.shell, b.atom, b.index);
          return py::iter(t);
        }
      )
      ;


    m.def("parse_basis_set_json", &parse_basis_set_json);


  }

}
}
