#include "bluejay/ga/ga.h"

#include "bluejay/core/pybind11.h"
#include "bluejay/core/eigen.h"
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <pybind11/embed.h>

#include <iostream>
#include <boost/config.hpp>

namespace bluejay {
namespace ga {

  py::slice slice_cast(py::handle s) {
    if (py::isinstance<py::int_>(s)) {
      int64_t i = py::cast<int64_t>(s);
      return py::slice(i, i+1, 1);
    }
    return py::slice(py::reinterpret_borrow<py::object>(s));
  }

  range<int64_t> range_cast(const int64_t &idx) {
    return range<int64_t>{ idx, idx+1 };
  }

  range<int64_t> range_cast(const py::slice &idx) {
    return {
      range<int64_t>(
        idx.attr("start").cast<int64_t>(),
        idx.attr("stop").cast<int64_t>()
      )
    };
  }

  std::vector< range<int64_t> > indices(const py::tuple &idx) {
    std::vector< range<int64_t> > v;
    for (const auto &i : idx) {
      if (py::isinstance<py::int_>(i)) {
        v.push_back(
          range_cast(py::cast<int64_t>(i))
        );
        continue;
      }
      v.push_back(
        range_cast(
          slice_cast(i)
        )
      );
    }
    return v;
  }

  // std::vector<py::slice> indices(py::object idx) {
  //   if (py::isinstance<py::slice>(idx)) {
  //     return { py::slice(idx) };
  //   }
  //   if (py::isinstance<py::int_>(idx)) {
  //     return { make_slice(idx) };
  //   }
  //   std::vector<py::slice> v;
  //   for (auto i : idx) {
  //     v.push_back(make_slice(i));
  //   }
  //   return v;
  // }

  template<typename U>
  auto  __getitem__(const GlobalArray &ga, const U &idx);

  // template<typename U>
  // auto  __getitem__(const GlobalArray &ga, const U &idx) {
  //   //py::print("__getitem__", idx);
  //   py::array_t<double, py::array::f_style> a(ga.shape());
  //   //ga.get(indices(idx), a.mutable_data());
  //   return a;
  // }

  template<>
  auto __getitem__(const GlobalArray &ga, const int64_t &idx) {
    //py::print("__getitem__", idx);
    return ga.get({ idx });
  }

  template<>
  auto __getitem__(const GlobalArray &ga, const std::vector<int64_t> &idx) {
    //py::print("__getitem__", idx);
    return ga.get(idx);
  }

  template<>
  auto  __getitem__(const GlobalArray &ga, const py::tuple &idx) {
    //py::print("__getitem__", idx);
    const auto &shape = ga.shape();
    assert(shape.size() == idx.size());
    std::vector< range<int64_t> > r;
    std::vector<size_t> dims;
    for (size_t i = 0; i < idx.size(); ++i) {
      if (py::isinstance<py::int_>(idx[i])) {
        int64_t s = py::cast<int>(idx[i]);
        r.push_back(range<int64_t>{s, s+1});
        continue;
      }
      const auto &s = py::cast<py::slice>(idx[i]);
      size_t start, stop, step, size;
      s.compute(shape.at(i), &start, &stop, &step, &size);
      r.emplace_back(
        int64_t(start),
        int64_t(stop)
      );
      //py::print("range", i, start, stop);
      dims.push_back(size);
    }
    py::array_t<double> a(dims);
    py::buffer_info buffer = a.request();
    std::vector<int64_t> strides = buffer.strides;
    for (size_t i = 0; i < buffer.strides.size(); ++i) {
      strides[i] /= sizeof(double);
    }
    ga.get(r, buffer.ptr, strides);
    //py::print("strides", buffer.strides);
    return a;
  }

  template<>
  auto  __getitem__(const GlobalArray &ga, const py::slice &idx) {
    return __getitem__(ga, py::tuple(idx));
  }


  template<typename U, class B>
  void __setitem__(GlobalArray &ga, const U &idx, B b);

  template<>
  void __setitem__(GlobalArray &ga, const std::vector<int64_t> &idx, double b) {
    //py::print("__setitem1__");
    ga.set(idx, b);
  }

  template<>
  void __setitem__(GlobalArray &ga, const int64_t &idx, double b) {
    //py::print("__setitem1__");
    ga.set({ idx }, b);
  }

  template<>
  void __setitem__(GlobalArray &ga, const py::tuple &idx, py::buffer b) {
    py::buffer_info buffer = b.request();
    // py::print("__setitem__", idx, buffer.ptr);
    // std::cout << "__setitem__: " << buffer.ptr << std::endl;
    std::vector<int64_t> strides;
    for (auto s : buffer.strides) {
      strides.push_back(s/sizeof(double));
    }
    ga.set(indices(idx), buffer.ptr, strides);
  }

  template<>
  void __setitem__(GlobalArray &ga, const py::slice &idx, py::buffer buffer) {
  }


  PYBIND11_EMBEDDED_MODULE(bluejay_ga, m) {

    py::class_<GlobalArray>(m, "GlobalArray")
      .def(py::init<const std::vector<size_t>&>())

      .def("__getitem__", &__getitem__<int64_t>)
      .def("__getitem__", &__getitem__<std::vector<int64_t> >)
      .def("__getitem__", &__getitem__<py::slice>)
      .def("__getitem__", &__getitem__<py::tuple>)

      .def("__setitem__", &__setitem__<int64_t,              double>)
      .def("__setitem__", &__setitem__<std::vector<int64_t>, double>)
      .def("__setitem__", &__setitem__<py::slice,            py::buffer>)
      .def("__setitem__", &__setitem__<py::tuple,            py::buffer>)

      .def_property_readonly("shape", &GlobalArray::shape)

      ;

  }

}
}
