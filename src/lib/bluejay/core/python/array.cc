//#include <iostream>
#include "bluejay/core/array.h"
#include "bluejay/core/pybind11.h"
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace bluejay {
namespace array {

  Range<int64_t> range_cast(const py::int_ &idx) {
    int64_t s = py::cast<int64_t>(idx);
    return Range<int64_t>{ s, s+1 };
  }

  Range<int64_t> range_cast(const py::slice &idx) {
    return {
      Range<int64_t>(
        idx.attr("start").cast<int64_t>(),
        idx.attr("stop").cast<int64_t>()
      )
    };
  }

  inline std::vector< Range<int64_t> > transform_to_range_vector(const py::sequence &idx) {
    std::vector< Range<int64_t> > v;
    for (const auto &i : idx) {
      if (py::isinstance<py::int_>(i)) {
        v.push_back(
          range_cast(py::int_(i))
        );
        continue;
      }
      py::slice s = py::slice(py::reinterpret_borrow<py::object>(i));
      v.push_back(range_cast(s));
    }
    return v;
  }

  template<typename Idx>
  inline std::vector< Range<int64_t> > transform_to_range_vector(const Idx &idx) {
    //py::print("transform_to_range_vector: ", idx);
    return { range_cast(idx) };
  }

  inline std::vector< Range<int64_t> > transform_to_range_vector(const std::vector< Range<int64_t> > &idx) {
    return idx;
  }

  inline py::list transform_to_slice_list(const std::vector< Range<int64_t> > &idx) {
    py::list list(idx.size());
    for (size_t i = 0; i < idx.size(); ++i) {
      list[i] = py::slice(idx[i].start, idx[i].stop, 1);
    }
    return list;
  }

  std::vector<size_t> extents(const Array &A, const std::vector< Range<int64_t> > idx) {
    std::vector<size_t> v;
    for (auto r : idx) {
      v.push_back(r.stop-r.start);
    }
    return v;
  }

  inline py::array make_py_array(const Array::Buffer &b) {
    return py::array(
      py::dtype(b.dtype),
      b.shape,
      b.strides,
      b.ptr
    );
  }

  inline Array::Buffer make_buffer(py::array A) {
    py::buffer_info buffer = A.request();
    return Array::Buffer(buffer.format, buffer.ptr, buffer.shape, buffer.strides);
  }

  inline py::array array1(std::string dtype, size_t ndims) {
    return py::array(py::dtype(dtype), std::vector<int64_t>(1, ndims));
  }

  inline py::tuple index0(size_t ndims) {
    return py::cast(std::vector<int64_t>(0, ndims));
  }


  template<typename Idx>
  py::array __getitem__(const Array &A, const Idx &idx) {
    //py::print("__getitem__", py::cast(A), idx);
    const auto &r = transform_to_range_vector(idx);
    py::array b(py::dtype(A.dtype()), extents(A, r));
    A.get(r, make_buffer(b));
    return b;
  }


  template<typename Idx>
  void __setitem__(Array &A, const Idx &idx, const py::object &v) {
    //py::print("__setitem__", py::cast(A), idx);
    py::array b;
    if (py::isinstance<py::buffer>(v)) {
      b = py::array(py::cast<py::buffer>(v));
    }
    else {
      // assuming scalar
      b = array1(A.dtype(), A.ndim());
      b[index0(A.ndim())] = v;
    }
    A.set(transform_to_range_vector(idx), make_buffer(b));
  }

  struct PyArray : Array {

    explicit PyArray(py::array b)
      : Array(b.request().format, b.request().shape),
        array_(b)
    {
      //debug::print("PyArray: array.shape=%s; strides=%s", b.request().shape, b.request().strides);
    }

    ~PyArray() {}

  protected:

    void get(const std::vector< Range<int64_t> > &idx, const Array::Buffer &buffer) const override {
      make_py_array(buffer) = array_[py::tuple(transform_to_slice_list(idx))];
    }

    void set(const std::vector< Range<int64_t> > &idx, const Array::Buffer &buffer) override {
      array_[py::tuple(transform_to_slice_list(idx))] = make_py_array(buffer);
    }

  private:
    py::array array_;

  };

  std::shared_ptr<Array> make_array(py::array b) {
    return std::make_shared<PyArray>(b);
  }

  void init(py::module m) {

    py::class_<Array, std::shared_ptr<Array> >(m, "Array")

      //.def(py::init<std::string, const std::vector<int64_t>&>())
      .def(py::init(&make_array))

      .def("__getitem__", &__getitem__<py::sequence>)
      .def("__getitem__", &__getitem__<py::slice>)
      .def("__getitem__", &__getitem__<py::int_>)

      .def("__setitem__", &__setitem__<py::sequence>)
      .def("__setitem__", &__setitem__<py::slice>)
      .def("__setitem__", &__setitem__<py::int_>)

      .def_property_readonly("shape", &Array::dimensions)

      ;

    py::implicitly_convertible<py::array, Array>();

  }

}
}
