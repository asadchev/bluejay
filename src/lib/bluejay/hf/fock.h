#ifndef BLUEJAY_HF_FOCK_H
#define BLUEJAY_HF_FOCK_H

#include "bluejay/core/eigen.h"
#include "bluejay/chem/molecule.h"
#include "bluejay/chem/integral/engine.h"

namespace bluejay {
namespace hf {

  struct FockEngine {

    FockEngine(const Basis &basis, std::shared_ptr<IntegralEngine> integral_engine);

    static std::tuple< Matrix<double>, Matrix<double> > diagonalizer(const Matrix<double> &S);

    Matrix<double> overlap() const;
    Matrix<double> kinetic() const;
    Matrix<double> nuclear() const;

    Matrix<double> fock2e(Eigen::Ref< const Matrix<double> > D) const;

  private:
    Basis basis_;
    std::shared_ptr<IntegralEngine> integral_engine_;

  };

}
}

#endif /* BLUEJAY_HF_FOCK_H */
