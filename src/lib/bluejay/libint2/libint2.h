#ifndef BLUEJAY_LIBINT2_LIBINT2_H
#define BLUEJAY_LIBINT2_LIBINT2_H

#include "bluejay/chem/integral/engine.h"

namespace bluejay {
namespace libint2 {

  void initialize();

  struct LibintEngine : IntegralEngine {

    class OneBodyEngine;
    class TwoBodyEngine;

    LibintEngine(size_t max_nprim, int max_l)
      : max_nprim_(max_nprim), max_l_(max_l)
    {
    }

    void initialize() override;

    std::shared_ptr<IntegralEngine::OneBodyEngine> kinetic() const override;
    std::shared_ptr<IntegralEngine::OneBodyEngine> nuclear() const override;
    std::shared_ptr<IntegralEngine::TwoBodyEngine> coulomb() const override;

  private:
    size_t max_nprim_ = 0;
    int max_l_ = 0;

  };

}
}

#endif /* BLUEJAY_LIBINT2_LIBINT2_H */
