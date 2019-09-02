#include "bluejay/hf/fock.h"
#include "bluejay/core/debug.h"

#include <Eigen/Eigenvalues>

namespace bluejay {
namespace hf {

  enum { IJ, IK, IL, JK, JL, KL };

  struct fock_2e_kernel {

    typedef std::array<double,20*20> Tile;
    Tile F[6], D[6];
    //std::vector<double> V;

    fock_2e_kernel() {
      //V.resize(10*10*10*10);
    }

    bool compute(
      IntegralEngine::TwoBodyEngine &eri,
      const Basis::Shell &p,
      const Basis::Shell &q,
      const Basis::Shell &r,
      const Basis::Shell &s,
      const Matrix<double> &D)
    {

      const auto *V = eri.compute(p, q, r, s);
      if (!V) return false;

      init_tiles<IJ>(slice(D, p, q));
      init_tiles<IK>(slice(D, p, r));
      init_tiles<IL>(slice(D, p, s));
      init_tiles<JK>(slice(D, q, r));
      init_tiles<JL>(slice(D, q, s));
      init_tiles<KL>(slice(D, r, s));

      size_t np = nbf(p);
      size_t nq = nbf(q);
      size_t nr = nbf(r);
      size_t ns = nbf(s);

      // if (compute<1,1>(V, np, nq, nr, ns)) {
      //   return true;
      // }
      // if (compute<1>(V, np, nq, nr, ns)) {
      //   return true;
      // }
      // if (compute<3>(V, np, nq, nr, ns)) {
      //   return true;
      // }

      compute(V, np, nq, nr, ns);
      return true;

    }

    template<int IJ, class Block>
    void init_tiles(const MatrixBase<Block> &block) {
      auto *D = this->D[IJ].data();
      auto *F = this->F[IJ].data();
      size_t rows = (size_t)block.rows();
      size_t cols = (size_t)block.cols();
      std::fill(F, F+rows*cols, 0.0);
      for (size_t j = 0; j < cols; ++j) {
        for (size_t i = 0; i < rows; ++i) {
          *(D++) = block(i,j);
        }
      }
    }

    template<class Block>
    void store_tile(const Tile &tile, MatrixBase<Block> &&block) {
      auto * __restrict__ it = &tile[0];
      for (size_t j = 0; j < (size_t)block.cols(); ++j) {
        for (size_t i = 0; i < (size_t)block.rows(); ++i) {
          block(i,j) = *(it++);
        }
      }
    }

    template<int P = 0, int Q = 0, int R = 0, int S = 0>
    bool compute(const double *V, size_t np, size_t nq, size_t nr, size_t ns) {
      if (P and P != np) return false;
      if (Q and Q != nq) return false;
      if (R and R != nr) return false;
      if (S and S != ns) return false;
      size_t NP = (P ? P : np);
      size_t NQ = (Q ? Q : nq);
      size_t NR = (R ? R : nr);
      size_t NS = (S ? S : ns);

// #define TILE(T,IDX) auto* __restrict__ T ## IDX = T[IDX].data()
//       TILE(F,IJ); TILE(F,IK); TILE(F,IL); TILE(F,JK); TILE(F,JL); TILE(F,KL);
//       TILE(D,IJ); TILE(D,IK); TILE(D,IL); TILE(D,JK); TILE(D,JL); TILE(D,KL);

// #define IJ(T) T ## IJ[i+j*NP]
// #define IK(T) T ## IK[i+k*NP]
// #define IL(T) T ## IL[i+l*NP]
// #define JK(T) T ## JK[j+k*NQ]
// #define JL(T) T ## JL[j+l*NQ]
// #define KL(T) T ## KL[k+l*NR]

#define IJ(T) T[IJ][i+j*NP]
#define IK(T) T[IK][i+k*NP]
#define IL(T) T[IL][i+l*NP]
#define JK(T) T[JK][j+k*NQ]
#define JL(T) T[JL][j+l*NQ]
#define KL(T) T[KL][k+l*NR]

      const auto* v = V;

      for (size_t l = 0; l < NS; ++l) {
        for (size_t k = 0; k < NR; ++k) {
          for (size_t j = 0; j < NQ; ++j) {
            for (size_t i = 0; i < NP; ++i) {
              IJ(F) += *v*KL(D);
              IK(F) += *v*JL(D);
              IL(F) += *v*JK(D);
              JK(F) += *v*IL(D);
              JL(F) += *v*IK(D);
              KL(F) += *v*IJ(D);
              ++v;
            }
          }
        }
      }

      return true;

    }

    void update(Matrix<double> &F, Range<size_t> p, Range<size_t> q, Range<size_t> r, Range<size_t>  s) {
      store_tile(this->F[IJ], slice(F,p,q));
      store_tile(this->F[IK], slice(F,p,r));
      store_tile(this->F[IL], slice(F,p,s));
      store_tile(this->F[JK], slice(F,q,r));
      store_tile(this->F[JL], slice(F,q,s));
      store_tile(this->F[KL], slice(F,r,s));
    }



  private:
    //range<size_t> ri, rj, rk, rl;
      // Matrix<double> F_ij, F_kl;
  };


  inline Matrix<double> h1(const Basis &basis, std::shared_ptr<IntegralEngine::OneBodyEngine> &&int1e) {
    assert(int1e);
    Matrix<double> h1(nbf(basis), nbf(basis));
    h1.setZero();
    for (size_t i = 0; i < basis.size(); ++i) {
      for (size_t j = 0; j <= i; ++j) {
        auto *h = int1e->compute(basis[i], basis[j]);
        if (!h) continue;
        auto hij = Matrix<double>::Map(h, nbf(basis[i]), nbf(basis[j]));
        slice(h1, basis[i], basis[j]) = hij;
        if (i == j) continue;
        slice(h1, basis[j], basis[i]) = hij.transpose();
      }
    }
    //std::cout << h1 << std::endl;
    return h1;
  }

  FockEngine::FockEngine(const Basis &basis, std::shared_ptr<IntegralEngine> integral_engine)
    : basis_(basis),
      integral_engine_(integral_engine)
  {
    debug::print("FockEngine: nbf=%i, shells=%i", basis.nbf(), basis.size());
    assert(integral_engine);
  }

  Matrix<double> FockEngine::overlap() const {
    return h1(this->basis_, this->integral_engine_->overlap(&this->basis_));
  }

  Matrix<double> FockEngine::kinetic() const {
    return h1(this->basis_, this->integral_engine_->kinetic(&this->basis_));
  }

  Matrix<double> FockEngine::nuclear() const {
    return h1(this->basis_, this->integral_engine_->nuclear(&this->basis_));
  }

  std::tuple< Matrix<double>, Matrix<double> > FockEngine::diagonalizer(const Matrix<double> &S) {
    Eigen::SelfAdjointEigenSolver< Matrix<double> > eigensystem(S);
    const auto &U = eigensystem.eigenvectors();
    const auto &s = eigensystem.eigenvalues();

    const auto &sigma = s;
    auto sigma_sqrt = sigma.array().sqrt().matrix().asDiagonal();
    auto sigma_invsqrt = sigma.array().sqrt().inverse().matrix().asDiagonal();

    Matrix<double> X = U*sigma_invsqrt;
    Matrix<double> Xinv = U*sigma_sqrt;

    return std::make_tuple(X, Xinv);
  }


  Matrix<double> FockEngine::fock2e(Eigen::Ref< const Matrix<double> > D) const {

    const auto &basis = this->basis_;
    Matrix<double> F = Matrix<double>::Zero(nbf(basis),nbf(basis));

    // //#pragma omp parallel
    // {

    //   auto eri = this->integral_engine_->coulomb(&basis);
    //   fock_2e_kernel kernel;

    //   for (size_t i = 0; i < basis.size(); ++i) {

    //     for (size_t j = 0; j < i+1; ++j) {

    //       //#pragma omp for schedule(dynamic,1)
    //       for (size_t k = 0; k < j+1; ++k) {
    //         size_t L = (i == k) ? j : k;
    //         for (size_t l = 0; l < L+1; ++l) {

    //           auto p = basis[i];
    //           auto q = basis[j];
    //           auto r = basis[k];
    //           auto s = basis[l];

    //           if (!kernel.compute(*eri, p, q, r, s, D)) continue;

    //           // i,j i,k i,l j,k, j,l k,l
    //           kernel.store_tile(kernel.F[IJ], slice(F,p,q));
    //           kernel.store_tile(kernel.F[IK], slice(F,p,r));
    //           kernel.store_tile(kernel.F[IL], slice(F,p,s));
    //           kernel.store_tile(kernel.F[JK], slice(F,q,r));
    //           kernel.store_tile(kernel.F[JL], slice(F,q,s));
    //           kernel.store_tile(kernel.F[KL], slice(F,r,s));

    //         }

    //       }
    //       // lock + update(F[J*]
    //       // #pragma omp barrier

    //     }
    //     // lock + update(F[I*]
    //   }

    // }

    auto eri = this->integral_engine_->coulomb(&basis);
    for (size_t i = 0; i < basis.size(); ++i) {
      for (size_t j = 0; j < basis.size(); ++j) {
        for (size_t k = 0; k < basis.size(); ++k) {
          for (size_t l = 0; l < basis.size(); ++l) {

            const auto& P = basis[i];
            const auto& Q = basis[j];
            const auto& R = basis[k];
            const auto& S = basis[l];

            const auto *V = eri->compute(P, Q, R, S);
            if (!V) continue;

            for (size_t p = 0; p < nbf(P); ++p) {
              for (size_t q = 0; q < nbf(Q); ++q) {
                for (size_t r = 0; r < nbf(R); ++r) {
                  for (size_t s = 0; s < nbf(S); ++s) {
                    F(p+P.index,q+Q.index) += *V*2*D(r+R.index,s+S.index);
                    ++V;
                  }
                }
              }
            }

            V = eri->compute(P, R, Q, S);

            for (size_t p = 0; p < nbf(P); ++p) {
              for (size_t r = 0; r < nbf(R); ++r) {
                for (size_t q = 0; q < nbf(Q); ++q) {
                  for (size_t s = 0; s < nbf(S); ++s) {
                    F(p+P.index,q+Q.index) -= *V*D(r+R.index,s+S.index);
                    ++V;
                  }
                }
              }
            }

          }
        }
      }
    }

    return F;

  }


}
}
