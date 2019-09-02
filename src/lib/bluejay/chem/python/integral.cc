#include "bluejay/chem/integral/engine.h"

#include "bluejay/core/pybind11.h"
#include <pybind11/stl.h>
#include <pybind11/eigen.h>

namespace bluejay {
namespace integral {

  struct PyIntegralEngine : IntegralEngine {

    typedef IntegralEngine::Shell Shell;

    struct TwoBodyEngine : IntegralEngine::TwoBodyEngine {

      const double* compute(
        const Shell &a,
        const Shell &b,
        const Shell &c,
        const Shell &d) override
      {
        //py::print("PyIntegralEngine::ERI::compute");
        PYBIND11_OVERLOAD_PURE(
          const double*,
          IntegralEngine::TwoBodyEngine,
          compute
        );
      }

    };

    void initialize() override {
      PYBIND11_OVERLOAD_PURE(
        void,
        IntegralEngine,
        initialize
      );
    }

    std::shared_ptr<IntegralEngine::TwoBodyEngine> coulomb(const Basis *basis) const override {
      PYBIND11_OVERLOAD(
        std::shared_ptr<TwoBodyEngine>,
        IntegralEngine,
        coulomb,
        basis
      );
    }

  };

  inline py::object two_body_integral_compute(
    IntegralEngine::TwoBodyEngine &int2e,
    const IntegralEngine::Shell& p,
    const IntegralEngine::Shell& q,
    const IntegralEngine::Shell& r,
    const IntegralEngine::Shell& s)
  {
    const auto *result = int2e.compute(p,q,r,s);
    if (!result) return py::none();
    auto shape = { nbf(p), nbf(q), nbf(r), nbf(s) };
    return py::array(shape, result);
  }

  void init(py::module m) {

    auto integral_engine = py::class_<IntegralEngine, PyIntegralEngine, std::shared_ptr<IntegralEngine> >(m, "IntegralEngine")
      .def(py::init<>())
      .def("initialize", &IntegralEngine::initialize)
      .def("coulomb", &IntegralEngine::coulomb, py::arg("basis") = py::none())
      .def_static("test_extension", [](const IntegralEngine&) { return true; })
      // .def_static("test_eri_extension", [](IntegralEngine &e) { return e.coulomb(); })
      // .def_property_static(
      //   "default_instance",
      //   [](py::object) {
      //     return IntegralEngine::default_instance();
      //   },
      //   [](py::object, std::shared_ptr<IntegralEngine> e) {
      //     IntegralEngine::set_default_instance(e);
      //   }
      // )
      ;

    py::class_<
      IntegralEngine::TwoBodyEngine,
      PyIntegralEngine::TwoBodyEngine,
      std::shared_ptr< IntegralEngine::TwoBodyEngine >
      >(integral_engine, "TwoBodyEngine")
      .def(py::init<>())
      .def("compute", &two_body_integral_compute)
      ;


  }

}
}
