#include "bluejay/core//pybind11.h"
#include "bluejay/core/python/json.h"

#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <boost/config.hpp>

#include "bluejay/chem/basis.h"

namespace bluejay {
namespace basis {

  std::unique_ptr<Shell> make_basis_shell(std::string s, const std::vector<Shell::Primitive> &p) {
    return std::make_unique<Shell>(s, p);
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

  inline std::string __repr__(const Shell &s) {
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

  void init(py::module m) {

    auto PyShell = py::class_<Shell, std::shared_ptr<Shell> >(m, "Shell")
      .def(py::init(&make_basis_shell))
      .def("__repr__", __repr__)
      .def_property_readonly("data", &Shell::data)
      .def_property_readonly("L", &Shell::L)
      .def("__getitem__", __getitem__<Shell,size_t>)
      .def("__iter__", &iter<Shell>)
      ;

    auto PyBasisSet = py::class_<BasisSet, std::shared_ptr<BasisSet> >(m, "BasisSet")
      .def(py::init<std::string, std::string>(), py::arg("name"), py::arg("path") = "")
      .def(py::init(&make_basis_set))
      .def("__getitem__", __getitem__<BasisSet,std::string>)
      .def("__iter__", &iter<BasisSet>)
      ;

    py::implicitly_convertible<py::str, BasisSet>();

    PyBasisSet.attr("Shell") = PyShell;
    m.def("parse_basis_set_json", &parse_basis_set_json);


  }

}
}
