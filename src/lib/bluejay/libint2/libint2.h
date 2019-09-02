#ifndef BLUEJAY_LIBINT2_LIBINT2_H
#define BLUEJAY_LIBINT2_LIBINT2_H

#include "bluejay/chem/integral/engine.h"

namespace bluejay {
namespace libint2 {

  void initialize();

  struct LibintEngine : IntegralEngine {

    class OneBodyEngine;
    class TwoBodyEngine;

    void initialize() override;

    std::shared_ptr<IntegralEngine::OneBodyEngine> overlap(const Basis*) const override;
    std::shared_ptr<IntegralEngine::OneBodyEngine> kinetic(const Basis*) const override;
    std::shared_ptr<IntegralEngine::OneBodyEngine> nuclear(const Basis*) const override;
    std::shared_ptr<IntegralEngine::TwoBodyEngine> coulomb(const Basis*) const override;

  };

}
}

#endif /* BLUEJAY_LIBINT2_LIBINT2_H */
