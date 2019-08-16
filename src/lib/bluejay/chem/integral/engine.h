#ifndef BLUEJAY_INTEGRAL_ENGINE_H
#define BLUEJAY_INTEGRAL_ENGINE_H

#include "bluejay/core/eigen.h"
#include "bluejay/chem/basis.h"

namespace bluejay {
namespace integral {

  struct IntegralEngine {

    struct OneBodyEngine;
    struct TwoBodyEngine;

    virtual void initialize() {}

    virtual std::shared_ptr<OneBodyEngine> overlap() const { return NULL; }
    virtual std::shared_ptr<OneBodyEngine> kinetic() const { return NULL; }
    virtual std::shared_ptr<OneBodyEngine> nuclear() const { return NULL; }
    virtual std::shared_ptr<TwoBodyEngine> coulomb() const { return NULL; }

  //   static std::shared_ptr<IntegralEngine> default_instance() {
  //     return default_instance_;
  //   }

  //   static void set_default_instance(std::shared_ptr<IntegralEngine> e) {
  //     default_instance_ = e;
  //   }

  // private:
  //   static std::shared_ptr<IntegralEngine> default_instance_;

  };

  struct IntegralEngine::OneBodyEngine {

    virtual bool compute(
      std::tuple<const Shell&, const Vector3<double>&> a,
      std::tuple<const Shell&, const Vector3<double>&> b,
      Matrix<double> &q
    ) = 0;

  };

  struct IntegralEngine::TwoBodyEngine {

    virtual bool compute(
      std::tuple<const Shell&, const Vector3<double>&> a,
      std::tuple<const Shell&, const Vector3<double>&> b,
      std::tuple<const Shell&, const Vector3<double>&> c,
      std::tuple<const Shell&, const Vector3<double>&> d,
      Tensor4d &v
    ) = 0;

  };

}
}

namespace bluejay {

  using bluejay::integral::IntegralEngine;

}

#endif /* BLUEJAY_INTEGRAL_ENGINE_H */
