#include "bluejay/chem/integral/transform.h"
#include "bluejay/chem/integral/transform/expression.h"
#include "bluejay/core/math/einsum.h"

namespace bluejay {
namespace integral {
namespace transform {

  void IntegralTransform::transform(
    std::string expr,
    Tensor4d &t,
    const Matrix<double> &c1,
    const Matrix<double> &c2) const
  {
    auto expression = TransformExpression(expr);
    expression.Validate(t, c1, c2);
  }

  // void TransformExpression::validate(const std::vector<std::string> &symbols) const {
  //   std::set<std::string> s;
  //   s.insert(lhs.symbol);
  //   for (auto term : rhs) {
  //     if (const auto *t = boost::get<Tensor>(&term)) {
  //       s.insert(t->symbol);
  //     }
  //   }
  //   if (s != std::set<std::string>(symbols.begin(), symbols.end())) {
  //     throw std::runtime_error(
  //       "Invalid expression arguments: "
  //       "expected " + join(",", s.begin(), s.end())
  //     );
  //   }
  // }

  // template<size_t N, typename Index = size_t>
  // struct index_permutation {
  //   index_permutation(std::array<size_t,N> p, std::array<Index,N> idx)
  //     : permutation_(p), indicies_(idx)
  //   {
  //   }
  //   Index operator[](size_t i) const {
  //     return indicies_[permutation_[i]];
  //   }
  // private:
  //   std::array<size_t,N> permutation_;
  //   std::array<Index,N> indicies_;
  // };

  // void transform_2_index(
  //   std::array<size_t,4> type,
  //   IntegralEngine::TwoBodyEngine &int2e,
  //   const Molecule::Basis& basis,
  //   std::array<size_t,2> ket,
  //   Tensor4d &V)
  // {
  //   //static_assert((Type == 0213 || Type == 0123), "");
  //   for (size_t q = 0; q < basis.size(); ++q) {
  //     for (size_t p = 0; p < basis.size(); ++p) {
  //       index_permutation<4> quartet(type, { p, q, ket[0], ket[1] });
  //       auto nbf = [&] (size_t i) {
  //         return basis::nbf(basis[quartet[i]].shell());
  //       };
  //       Tensor4d v(nbf(0), nbf(1), nbf(2), nbf(3));
  //       auto shell = [&] (size_t i) {
  //         return std::tie(basis[quartet[i]].shell(), basis[quartet[i]].center());
  //       };
  //       //int2e.compute(shell(0), shell(1), shell(2), shell(3), v);
  //     }
  //   }
  // }


  void IntegralTransform::compute_pq(size_t k, size_t l) {
    // const auto &basis = this->basis_;
    // // const auto &r = basis[k];
    // // const auto &s = basis[l];
    // for (const auto &p : basis) {
    //   for (const auto &q : basis) {
    //     //Tensor4d eri;
    //     //integral_engine_->compute(p, q, r, s, eri);
    //     //eri_(r,s,p,q) = eri;
    //   }
    // }
  }

  Tensor4d IntegralTransform::transform_pq(const Matrix<double> &Ci, const Matrix<double> &Cj) const {
    // const auto &V = this->eri_;
    // Tensor4d t1, t2;
    // einsum::einsum(Cj, 1, V, 3, t1); // u(j,r,s,p) = C(j,q)*v(r,s,p,q)
    // einsum::einsum(Ci, 1, V, 3, t2); // t(i,j,r,s) = C(i,p)*u(j,r,s,p)
    //return t2;
    return Tensor4d{};
  }

  Tensor4d IntegralTransform::transform_pq(const Tensor4d &Tij) const {
    //const auto &V = this->eri_;
    //auto t2 = einsum("ijpq,rspq", Tij, V); // t(i,j,r,s) = T(i,j,p,q)*v(r,s,p,q)
    return Tensor4d{};
  }

}
}
}
