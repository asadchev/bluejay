#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <boost/optional/optional_fwd.hpp>
#include <boost/variant/variant_fwd.hpp>

namespace pybind11 {
namespace detail {

  template <typename T>
  struct type_caster< boost::optional<T> >
    : optional_caster< boost::optional<T> >
  {
  };

  template <typename... Ts>
  struct type_caster< boost::variant<Ts...> >
    : variant_caster< boost::variant<Ts...> >
  {
  };

}
}

namespace bluejay {

  namespace py = pybind11;

  inline std::string str(const py::handle &h) {
    return py::cast<std::string>(py::repr(h));
  }

}
