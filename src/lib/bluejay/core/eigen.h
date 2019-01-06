#ifndef BLUEJAY_CORE_EIGEN_H
#define BLUEJAY_CORE_EIGEN_H

#include <Eigen/Core>
#include <unsupported/Eigen/CXX11/Tensor>
#include "bluejay/core/range.h"
#include "bluejay/core/debug.h"

namespace bluejay {

  using Eigen::MatrixBase;

  template<typename T, Eigen::StorageOptions StorageOrder = Eigen::RowMajor>
  using Matrix = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, StorageOrder>;

  template<typename T, int N = Eigen::Dynamic>
  using Vector = Eigen::Matrix<T, N, 1>;

  template<typename T>
  using Vector3 = Eigen::Matrix<T, 3, 1>;

  template<typename T, int N, Eigen::StorageOptions StorageOrder = Eigen::RowMajor>
  using Tensor = Eigen::Tensor<T,N,StorageOrder>;

  template<typename T>
  using Tensor4 = Tensor<T,4>;

  typedef Tensor<double,2> Tensor2d;
  typedef Tensor<double,4> Tensor4d;

  using Eigen::TensorRef;
  using Eigen::TensorBase;
  using Eigen::TensorMap;


  template<typename T, int Rows, int Cols, int Options>
  size_t rank(const Eigen::Matrix<T,Rows,Cols,Options> &m) {
    if (Rows == 1 && Cols == 1) {
      return 1;
    }
    return ((Rows != 1) + (Cols != 1));
  }

  template<typename T, int N, int Options>
  size_t rank(const Eigen::Tensor<T,N,Options> &m) {
    return N;
  }


  template<typename T, size_t K, int Order>
  inline std::vector<size_t> strides(const std::array<int64_t,K> &dims) {
    std::vector<size_t> s(K);
    for (size_t i = 0, stride = 1; i < K; ++i) {
      size_t k = -1;
      if (Order & Eigen::ColMajor) k = i;
      if (Order & Eigen::RowMajor) k = K-i-1;
      s.at(k) = stride*sizeof(T);
      stride *= dims.at(k);
    }
    return s;
  }

  template<typename T, int Rows, int Cols, int Options>
  inline std::vector<size_t> strides(const Eigen::Matrix<T,Rows,Cols,Options> &m) {
    return strides<T,2,Options>({ m.rows(), m.cols() });
  }

  template<typename T, int N, int Options>
  inline std::vector<size_t> strides(const Eigen::Tensor<T,N,Options> &m) {
    return strides<T,N,Options>(m.dimensions());
  }

  template<typename T, size_t N>
  Vector<T> as_vector(const std::array<T,N> &a) {
    Vector<T> v(N);
    for (size_t i = 0; i < N; ++i) {
      v[i] = a[i];
    }
    return v;
  }

  template<class B>
  auto slice(Eigen::MatrixBase<B> &m, Range<size_t> rows, Range<size_t> cols) {
    return m.block(rows.start, cols.start, rows.stop-rows.start, cols.stop-cols.start);
  }

  template<class B>
  auto slice(const Eigen::MatrixBase<B> &m, Range<size_t> rows, Range<size_t> cols) {
    return m.block(rows.start, cols.start, rows.stop-rows.start, cols.stop-cols.start);
  }

}

#include "bluejay/core/string.h"

namespace bluejay {

  template<typename T, int N, int Options>
  std::string str(const Eigen::Tensor<T,N,Options> &b) {
    const auto &dims = b.dimensions();
    return "Tensor[" + string::join(",", dims.begin(), dims.end()) + "]";
  }

  template<class T>
  std::string str(const Eigen::TensorMap<T> &b) {
    const auto &dims = b.dimensions();
    return "TensorMap[" + string::join(",", dims.begin(), dims.end()) + "]";
  }

  template<typename T>
  std::string str(const Matrix<T> &b) {
    return "Matrix[" + std::to_string(b.rows()) + "," + std::to_string(b.cols()) + "]";
  }

}

#endif // BLUEJAY_CORE_EIGEN_H
