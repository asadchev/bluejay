#include "bluejay/hf/hf.h"
#include "bluejay/hf/fock.h"
#include <Eigen/Eigenvalues>

namespace bluejay {
namespace hf {

  HartreeFock::HartreeFock(const Basis &basis, std::shared_ptr<IntegralEngine> integral_engine)
    : basis_(basis),
      integral_engine_(integral_engine)
  {
  }



  // Matrix<double> HartreeFock::guess_hcore() const {
  //   size_t nbf = this->basis_.nbf();
  //   return Matrix<double>::Identity(nbf,nbf);
  // }

  // double HartreeFock::update(const Matrix<double> &D) {
  //   FockEngine fock_engine(this->basis_, this->integral_engine_);
  //   Matrix<double> F = fock_engine.fock_2e(D);
  //   F = F + F.transpose();
  //   auto es = Eigen::SelfAdjointEigenSolver< Matrix<double> >(F);
  //   return es.eigenvalues().sum();
  // }

}
}
