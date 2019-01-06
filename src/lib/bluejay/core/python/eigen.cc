#include "bluejay/core//pybind11.h"
//#include <pybind11/eigen.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include "bluejay/core/eigen.h"
#include "bluejay/core/tuple.h"
#include <iostream>

namespace bluejay {
namespace eigen {

  // void init_matrix_buffer(Matrix<double> &m, py::buffer b) {

  //   typedef Matrix<double>::Scalar Scalar;
  //   typedef Eigen::Stride<Eigen::Dynamic, Eigen::Dynamic> Strides;
  //   constexpr bool rowMajor = Matrix<double>::Flags & Eigen::RowMajorBit;

  //   /* Request a buffer descriptor from Python */
  //   py::buffer_info info = b.request();

  //   /* Some sanity checks ... */
  //   if (info.format != py::format_descriptor<Scalar>::value)
  //     throw std::runtime_error("Incompatible format: expected a double array!");

  //   if (info.ndim != 2)
  //     throw std::runtime_error("Incompatible buffer dimension!");

  //   auto strides = Strides(
  //     info.strides[rowMajor ? 0 : 1] / sizeof(Scalar),
  //     info.strides[rowMajor ? 1 : 0] / sizeof(Scalar));

  //   auto map = Eigen::Map<Matrix<double>, 0, Strides>(
  //     static_cast<Scalar *>(info.ptr),
  //     info.shape[0],
  //     info.shape[1],
  //     strides
  //   );

  //   new (&m) Matrix<double>(map);
  // }

  template<typename T>
  py::buffer_info matrix_buffer(Matrix<T> &m) {
    return py::buffer_info(
      m.data(),                        // Pointer to buffer
      sizeof(T),                       // Size of one scalar
      py::format_descriptor<T>::value, // Python struct-style format descriptor
      2,                               // Number of dimensions
      { m.rows(), m.cols() },          // Buffer dimensions
      strides(m)                       // Strides (in bytes) for each index
    );
   }

  template<typename T, size_t N>
  py::buffer_info tensor_buffer(Tensor<T,N> &m) {
    return py::buffer_info(
      m.data(),                        // Pointer to buffer
      sizeof(T),                       // Size of one scalar
      py::format_descriptor<T>::value, // Python struct-style format descriptor
      2,                               // Number of dimensions
      m.dimensions(),                  // Buffer dimensions
      strides(m)                       // Strides (in bytes) for each index
    );
   }

  template<class O>
  typename O::Scalar __getitem__(const O&, const py::object &idx);

  template<class O>
  void __setitem1__(O&, const py::object &idx, typename O::Scalar v);

  template<class O>
  void __setitem__(O&, const py::object &idx, py::buffer b);

  template<size_t N>
  std::array<int64_t,N> indices(const py::object &idx) {
    return py::cast< std::array<int64_t,N> >(py::list(idx));
  }


  template<>
  double __getitem__(const Matrix<double> &o, const py::object &idx) {
    const auto &idx_ = indices<2>(idx);
    return o(idx_[0], idx_[1]);
  }

  template<>
  void __setitem1__(Matrix<double> &o, const py::object &idx, double v) {
    const auto &idx_ = indices<2>(idx);
    o(idx_[0], idx_[1]) = v;
  }

  template<typename T>
  py::object __repr__(const Matrix<T> &m) {
    auto buffer = matrix_buffer(const_cast< Matrix<T>& >(m));
    return py::str(py::array(buffer));
  }

  namespace tensor {

    template<class Tensor, int N>
    double __getitem__(const Tensor &o, const py::object &idx) {
      const auto &idx_ = indices<N>(idx);
      return o(idx_);
    }

    // template<typename Tensor, int N>
    // void __setitem1__(Tensor &o, const py::object &idx, double v) {
    //   const auto &idx_ = indices<N>(idx);
    //   o(idx_) = v;
    // }

    template<class Tensor, int N>
    void __setitem1__(Tensor &o, ntuple<int64_t,N> idx, double v) {
      // const auto &idx_ = indices<4>(idx);
      //o(idx_) = v;
    }

    template<class Tensor, int N>
    void __setitem__(Tensor &o, ntuple<py::object,N> idx, py::object v) {
      assert(0);
      // auto f = [](py::object o) {
      //   return py::cast<py::slice>(o);
      // };
      //py::print(tuple::transform(f, idx));
      // const auto &idx_ = indices<4>(idx);
      // o(idx_) = v;
    }

  }

  std::array<int64_t,4> shape(const Tensor4d &o) {
    const auto &dims = o.dimensions();
    return std::array<int64_t,4>(dims);
  }

  void init(py::module m) {

    py::class_<Matrix<double>>(m, "Matrix", pybind11::buffer_protocol())
      .def(py::init<size_t, size_t>())
      .def_property_readonly("rows", &Matrix<double>::rows)
      .def_property_readonly("cols", &Matrix<double>::cols)
      .def("__getitem__", __getitem__< Matrix<double> >)
      .def("__setitem__", __setitem1__< Matrix<double> >)
      .def("__repr__", __repr__<double>)
      .def_buffer(&matrix_buffer<double>)
      ;

    py::class_< Vector3<double> >(m, "Vector3", pybind11::buffer_protocol())
      .def(py::init<double,double,double>())
      ;

    py::class_<Tensor4d>(m, "Tensor4d", pybind11::buffer_protocol())
      .def(py::init())
      .def(py::init< int64_t, int64_t, int64_t, int64_t >())
      .def("shape", &shape)
      .def("resize", [](Tensor4d &t, const std::array<int64_t,4> &s) { t.resize(s); })
      .def("__getitem__", tensor::__getitem__<Tensor4d,4>)
      .def("__setitem__", tensor::__setitem1__<Tensor4d,4>)
      .def("__setitem__", tensor::__setitem__<Tensor4d,4>)
      .def_buffer(&tensor_buffer<double,4>)
      ;

  }

}
}
