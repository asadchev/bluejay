#include <tuple>

namespace bluejay {
namespace tuple {

  namespace detail {

    template<typename T, size_t N>
    struct ntuple;

    template<typename T>
    struct ntuple<T,0> {
      using type = std::tuple<>;
    };

    template<typename T>
    struct ntuple<T,1> {
      using type = std::tuple<T>;
    };

    template<typename T>
    struct ntuple<T,2> {
      using type = std::tuple<T,T>;
    };

    template<typename T>
    struct ntuple<T,3> {
      using type = std::tuple<T,T,T>;
    };

    template<typename T>
    struct ntuple<T,4> {
      using type = std::tuple<T,T,T,T>;
    };

    template<class F, typename ... Args, size_t ... Idx>
    auto transform(F &&f, const std::tuple<Args...> &t, std::index_sequence<Idx...>) {
      return std::make_tuple(
        f(std::get<Idx>(t))...
      );
    }

  }

  template<typename T, size_t N>
  using ntuple = typename tuple::detail::ntuple<T,N>::type;

  template<class F, class Tuple>
  auto transform(F &&f, Tuple &&t) {
    using index_sequence = std::make_index_sequence<
      std::tuple_size<std::decay_t<Tuple> >::value
      >;
    return tuple::detail::transform(f, t, index_sequence());
  }


}
}

namespace bluejay {

  using tuple::ntuple;

}
