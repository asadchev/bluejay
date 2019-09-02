#ifndef BLUEJAY_HF_HF_H
#define BLUEJAY_HF_HF_H

#include "bluejay/core/eigen.h"
#include "bluejay/chem/molecule.h"
#include "bluejay/chem/integral/engine.h"

namespace bluejay {
namespace hf {

  struct HartreeFock {

    HartreeFock(const Basis &basis, std::shared_ptr<IntegralEngine> integral_engine);

    Matrix<double> overlap() const;

    Matrix<double> Hcore() const;

    Matrix<double> guess_hcore() const;

    double update(const Matrix<double> &D);

  private:
    Basis basis_;
    std::shared_ptr<IntegralEngine> integral_engine_;

  };

}
}

#endif /* BLUEJAY_HF_HF_H */
