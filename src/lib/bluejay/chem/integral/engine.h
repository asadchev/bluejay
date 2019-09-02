#ifndef BLUEJAY_INTEGRAL_ENGINE_H
#define BLUEJAY_INTEGRAL_ENGINE_H

#include "bluejay/core/eigen.h"
#include "bluejay/chem/basis.h"

namespace bluejay {
namespace integral {

  using bluejay::basis::GaussianShell;

  struct IntegralEngine {

    typedef std::tuple<const GaussianShell&, const Vector3<double>&> Shell;

    struct OneBodyEngine;
    struct TwoBodyEngine;

    virtual ~IntegralEngine() {}
    virtual void initialize() {}

    virtual std::shared_ptr<OneBodyEngine> overlap(const Basis*) const { return NULL; }
    virtual std::shared_ptr<OneBodyEngine> kinetic(const Basis*) const { return NULL; }
    virtual std::shared_ptr<OneBodyEngine> nuclear(const Basis*) const { return NULL; }

    virtual std::shared_ptr<TwoBodyEngine> coulomb(const Basis*) const { return NULL; }

  //   static std::shared_ptr<IntegralEngine> default_instance() {
  //     return default_instance_;
  //   }

  //   static void set_default_instance(std::shared_ptr<IntegralEngine> e) {
  //     default_instance_ = e;
  //   }

  // private:
  //   static std::shared_ptr<IntegralEngine> default_instance_;

  };

  inline size_t nbf(const IntegralEngine::Shell &s) {
    return nbf(std::get<0>(s));
  }

  struct IntegralEngine::OneBodyEngine {

    virtual ~OneBodyEngine() {}

    virtual const double* compute(const Shell&, const Shell&) = 0;

  };

  struct IntegralEngine::TwoBodyEngine {

    virtual ~TwoBodyEngine() {}

    virtual const double* compute(const Shell&, const Shell&, const Shell&, const Shell&) = 0;

  };

}
}

namespace bluejay {

  using bluejay::integral::IntegralEngine;

}

#endif /* BLUEJAY_INTEGRAL_ENGINE_H */
