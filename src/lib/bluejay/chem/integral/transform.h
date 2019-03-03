#ifndef BLUEJAY_INTEGRAL_TRANSFORM_H
#define BLUEJAY_INTEGRAL_TRANSFORM_H

#include "bluejay/chem/basis.h"
#include "bluejay/chem/integral/index.h"
#include "bluejay/chem/integral/engine.h"
#include "bluejay/core/array.h"

#include <vector>
#include <string>
#include <boost/variant.hpp>
#include <boost/optional.hpp>

namespace bluejay {
namespace integral {
namespace transform {

  typedef boost::optional< std::tuple<Matrix<double>, IndexPair> > Transform1;
  typedef std::tuple<std::shared_ptr<Array>, IndexPair, IndexPair> Transform2;

  template<int N>
  struct IndexTransform;

  template<>
  struct IndexTransform<4> {

    std::shared_ptr<Array> T;
    std::array<size_t,4> order;

    boost::variant<
      std::tuple<Transform1,Transform1>,
      Transform2
      > T12;
    Transform1 T3;
    Transform1 T4;

    IndexTransform(
      std::shared_ptr<Array> T,
      std::array<size_t,4> order,
      Transform2 T12,
      Transform1 T3,
      Transform1 T4
    )
      : T(T), order(order), T12(T12), T3(T3), T4(T4)
    {
    }

    IndexTransform(
      std::shared_ptr<Array> T,
      std::array<size_t,4> order,
      Transform1 T1,
      Transform1 T2,
      Transform1 T3,
      Transform1 T4
    )
      : T(T), order(order), T12(std::make_tuple(T1,T2)), T3(T3), T4(T4)
    {
    }

  };


  void transform(
    const Basis &basis,
    IntegralEngine::TwoBodyEngine &int2e,
    std::array<size_t,4> integral_type,
    IndexTransform<4> transform
  );

}
}
}

namespace bluejay {

  //using bluejay::integral::transform::IntegralTransform;

}

#endif // BLUEJAY_INTEGRAL_TRANSFORM_H
