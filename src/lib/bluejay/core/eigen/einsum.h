#ifndef BLUEJAY_CORE_EIGEN_EINSUM_H
#define BLUEJAY_CORE_EIGEN_EINSUM_H

#include "bluejay/core/eigen.h"
#include "bluejay/core/debug.h"

namespace bluejay {
namespace einsum {

  template<typename T, int K>
  TensorMap< Tensor<T,K> > tensor_map(Tensor<T,K> &t) {
    return TensorMap< Tensor<T,K> >(t.data(), t.dimensions());
  }

  template<typename T, int K>
  TensorMap< Tensor<const T,K> > tensor_map(const Tensor<T,K> &t) {
    return TensorMap< Tensor<const T,K> >(t.data(), t.dimensions());
  }

  template<typename T>
  TensorMap< Tensor<const T,2> > tensor_map(const Matrix<T> &t) {
    return TensorMap< Tensor<const T,2> >(t.data(), { t.rows(), t.cols() });
  }

  template<typename T>
  Eigen::Map< Matrix<T> > matrix_map(T *data, size_t rows, size_t cols) {
    return Eigen::Map< Matrix<T> >(data, rows, cols);
  }

  template<typename T>
  Eigen::Map< const Matrix<T> > matrix_map(const T *data, size_t rows, size_t cols) {
    return Eigen::Map< const Matrix<T> >(data, rows, cols);
  }


  template<typename T>
  void einsum(
    Eigen::Map< const Matrix<T> > a, size_t i,
    Eigen::Map< const Matrix<T> > b, size_t j,
    Eigen::Map< Matrix<T> > c
  );

  template<>
  void einsum(
    Eigen::Map< const Matrix<double> > a, size_t i,
    Eigen::Map< const Matrix<double> > b, size_t j,
    Eigen::Map< Matrix<double> > c
  );

  template<typename T, int RankA, int RankB>
  void einsum(
    TensorMap< Tensor<const T,RankA> > A, size_t AxisA,
    TensorMap< Tensor<const T,RankB> > B, size_t AxisB,
    Tensor<T,(RankA-1 + RankB-1)> &C)
  {
    typedef std::pair<size_t,size_t> index_pair;
    //debug::print("einsum(%s, %i, %s, %i, %s)", str(A), AxisA, str(B), AxisB, str(C));
    C = A.contract(B, std::array<index_pair,1>{ index_pair(AxisA,AxisB) });
    // if ((AxisA == 0 || AxisA == RankA-1) && (AxisB == 0 || AxisB == RankB-1)) {
    //   size_t Am = 1, An = 1;
    //   for (size_t k = 0; k < AxisA+1; ++k) {
    //     Am *= A.dimension(k);
    //   }
    //   for (size_t k = AxisA+1; k < RankA; ++k) {
    //     An *= A.dimension(k);
    //   }
    //   size_t Bm = 1, Bn = 1;
    //   for (size_t k = 0; k < AxisB+1; ++k) {
    //     Bm = B.dimension(k);
    //   }
    //   for (size_t k =  AxisB+1; k < RankB; ++k) {
    //     Bn = B.dimension(k);
    //   }
    //   size_t Cm = 1, Cn = 1;
    //   for (size_t i = 0; i < RankA-1; ++i) {
    //     Cm *= C.dimension(i);
    //   }
    //   for (size_t i = RankA-1; i < RankA-1 + RankB-1; ++i) {
    //     Cm *= C.dimension(i);
    //   }
    //   einsum(
    //     matrix_map(B.data(), Am, An), (AxisA ? 1 : 0),
    //     matrix_map(B.data(), Bm, Bn), (AxisB ? 1 : 0),
    //     matrix_map(C.data(), Cm, Cn)
    //   );
    //   return;
    // }
    // assert("not implemented" && false);
    // return;
  }

  template<typename T, int K>
  void einsum(
    const Matrix<T> &a, size_t i,
    const Tensor<T,K> &b, size_t j,
    Tensor<T,K> &c)
  {
    return einsum(
      tensor_map(a), i,
      tensor_map(b), j,
      c
    );
  }

  // template<typename T, size_t K, size_t M, size_t N>
  // void einsum(
  //   const std::string &expression,
  //   TensorRef<const Tensor<T,K> > a,
  //   TensorRef<const Tensor<T,M> > b,
  //   TensorRef< Tensor<T,N> > c
  // );

}
}

#endif /* BLUEJAY_CORE_EIGEN_EINSUM_H */
