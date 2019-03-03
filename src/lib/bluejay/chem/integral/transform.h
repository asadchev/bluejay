#ifndef BLUEJAY_INTEGRAL_TRANSFORM_H
#define BLUEJAY_INTEGRAL_TRANSFORM_H

#include "bluejay/chem/molecule.h"
#include "bluejay/chem/integral/index.h"
#include "bluejay/chem/integral/engine.h"

#include <boost/variant.hpp>
#include <vector>
#include <string>

namespace bluejay {
namespace integral {
namespace transform {

  void transform_2_index(
    std::array<size_t,4> type,
    IntegralEngine::TwoBodyEngine &int2e,
    const Molecule::Basis& basis,
    std::array<size_t,2> ket,
    Tensor4d &V
  );

  struct TransformExpression {

    // eg "ijrs = pi*qj*(pq|rs)"
    explicit TransformExpression(const std::string &expr);

    template<class ... Args>
    void Validate(const Args& ... args) const {
      size_t n = (this->args().size());
      if (n != sizeof...(args)) {
        throw Exception("Transform '%s' expects %i arguments", this->str_, n);
      }
      std::vector<size_t> ranks = { rank(args)... };
      for (size_t i = 0; i < ranks.size(); ++i) {
        const auto term = this->args()[i];
        if (term.size() != ranks[i]) {
          throw Exception("Transform '%s' argument %i must be rank %i", this->str_, i, term.size());
        }
      }
    }

    std::string bra() const { return bra_; }
    std::string ket() const { return ket_; }
    std::vector<std::string> args() const { return args_; }

    template<size_t N>
    std::array<size_t,N> braket() const;

  private:
    std::string str_;
    std::string bra_, ket_;
    std::vector<std::string> args_;

  };

  void transform(
    const Molecule::Basis &basis,
    IntegralEngine::TwoBodyEngine &int2e,
    std::array<size_t,4> integral_type,
    std::tuple<const Matrix<double>&, IndexPair> C1,
    std::tuple<const Matrix<double>&, IndexPair> C2,
    Tensor4d &t,
    std::array<size_t,4> order
  );

  struct IntegralTransform {

    explicit IntegralTransform(
      const Molecule::Basis&,
      std::shared_ptr<IntegralEngine> = nullptr
    );

    const auto& basis() const { return basis_; }

    void compute_pq(size_t k, size_t l);
    const double* eri() const;
    Tensor4d transform_pq(const Matrix<double> &Ci, const Matrix<double> &Cj) const;
    Tensor4d transform_pq(const Tensor4d &Tij) const;

    void transform(
      std::string expr,
      Tensor4d &t,
      const Matrix<double> &c1,
      const Matrix<double> &c2
    ) const;

    void compute_pq(std::array<size_t,4> braket, size_t r, size_t s, Tensor4d &eri) const;

  private:
    Molecule::Basis basis_;
    std::shared_ptr<IntegralEngine> integral_engine_;
    Tensor4d eri_;
  };

}
}
}

namespace bluejay {

  using bluejay::integral::transform::IntegralTransform;

}

#endif // BLUEJAY_INTEGRAL_TRANSFORM_H
