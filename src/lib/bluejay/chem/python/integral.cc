#include "bluejay/chem/integral/engine.h"
#include "bluejay/chem/integral/transform.h"

#include "bluejay/core/pybind11.h"
#include <pybind11/stl.h>

namespace bluejay {
namespace integral {

  struct PyIntegralEngine : IntegralEngine {

    struct TwoBodyEngine : IntegralEngine::TwoBodyEngine {

      bool compute(
        std::tuple<const Shell&, const Vector3<double>&> a,
        std::tuple<const Shell&, const Vector3<double>&> b,
        std::tuple<const Shell&, const Vector3<double>&> c,
        std::tuple<const Shell&, const Vector3<double>&> d,
        Tensor4d &V) override
      {
        //py::print("PyIntegralEngine::ERI::compute");
        PYBIND11_OVERLOAD_PURE(
          bool,
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

    std::shared_ptr<IntegralEngine::TwoBodyEngine> coulomb() const override {
      PYBIND11_OVERLOAD(
        std::shared_ptr<TwoBodyEngine>,
        IntegralEngine,
        coulomb
      );
    }

  };

  void init(py::module m) {

    auto integral_engine = py::class_<IntegralEngine, PyIntegralEngine, std::shared_ptr<IntegralEngine> >(m, "IntegralEngine")
      .def(py::init<>())
      .def("initialize", &IntegralEngine::initialize)
      .def("coulomb", &IntegralEngine::coulomb)
      .def_static("test_extension", [](const IntegralEngine&) { return true; })
      .def_static("test_eri_extension", [](IntegralEngine &e) { return e.coulomb(); })
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
      .def("compute", &IntegralEngine::TwoBodyEngine::compute)
      ;

    py::class_<IntegralTransform>(m, "IntegralTransform")
      .def(
        py::init< const Molecule::Basis&,std::shared_ptr<IntegralEngine> >()
      )
      .def_property_readonly("basis", &IntegralTransform::basis)
      //.def("compute", &transform_compute)
      //.def("compute_pq", &IntegralTransform::compute_pq)
      .def(
        "transform_pq",
        [](IntegralTransform &transform, const Matrix<double> &C1, const Matrix<double> &C2) {
          return transform.transform_pq(C1, C2);
        }
      )
      .def(
        "transform_pq",
        [](IntegralTransform &transform, const Tensor4<double> &T12) {
          return transform.transform_pq(T12);
        }
      )
      ;

    m.def(
      "transform",
      transform::transform
    );

  }

}
}
