#include "bluejay/libint2/libint2.h"
#include <libint2.hpp>

namespace bluejay {
namespace libint2 {

  void initialize() {
    ::libint2::initialize();
  }

  void LibintEngine::initialize() {
    initialize();
  }

  static ::libint2::Shell shell(std::tuple<const Shell&, const Vector3<double>&> s) {
    int L = std::get<0>(s).L();
    const auto &p = std::get<0>(s).data();
    const auto &r = std::get<1>(s);
    ::libint2::svector<double> a(p.size());
    ::libint2::svector<double> c(p.size());
    for (size_t i = 0; i < p.size(); ++i) {
      a[i] = std::get<0>(p[i]);
      c[i] = std::get<1>(p[i]);
    }
    return ::libint2::Shell(
      a,
      {
        ::libint2::Shell::Contraction{L, true, c}
      }, // no sp shells yet
      { r[0], r[1], r[2] }
    );
  }

  struct LibintEngine::OneBodyEngine : IntegralEngine::OneBodyEngine {

    OneBodyEngine(::libint2::Operator op, size_t max_nprim, int max_l) {
      engine_.reset(new ::libint2::Engine(op, max_nprim, max_l));
    }

    bool compute(
      std::tuple<const Shell&, const Vector3<double>&> a,
      std::tuple<const Shell&, const Vector3<double>&> b,
      Matrix<double> &q) override
    {
      assert(false);
    }

  private:
    std::unique_ptr<::libint2::Engine> engine_;

  };

  struct LibintEngine::TwoBodyEngine : IntegralEngine::TwoBodyEngine {

    TwoBodyEngine(::libint2::Operator op, size_t max_nprim, int max_l) {
      engine_.reset(new ::libint2::Engine(op, max_nprim, max_l));
    }

    bool compute(
      std::tuple<const Shell&, const Vector3<double>&> a,
      std::tuple<const Shell&, const Vector3<double>&> b,
      std::tuple<const Shell&, const Vector3<double>&> c,
      std::tuple<const Shell&, const Vector3<double>&> d,
      Tensor4d &V) override
    {
      engine_->compute(shell(a), shell(b), shell(c), shell(d));
      using basis::nbf;
      V.resize(
        nbf<int64_t>(std::get<0>(d)),
        nbf<int64_t>(std::get<0>(c)),
        nbf<int64_t>(std::get<0>(b)),
        nbf<int64_t>(std::get<0>(a))
      );
      if (const double *buffer = engine_->results()[0]) {
        std::copy(buffer, buffer+V.size(), V.data());
        return true;
      }
      return false;
    }

  private:
    std::unique_ptr<::libint2::Engine> engine_;

  };

  std::shared_ptr<IntegralEngine::OneBodyEngine> LibintEngine::kinetic() const {
    return std::make_shared<LibintEngine::OneBodyEngine>(
      ::libint2::Operator::kinetic,
      this->max_nprim_,
      this->max_l_
    );
  }

  std::shared_ptr<IntegralEngine::OneBodyEngine> LibintEngine::nuclear() const {
    return std::make_shared<LibintEngine::OneBodyEngine>(
      ::libint2::Operator::nuclear,
      this->max_nprim_,
      this->max_l_
    );
  }

  std::shared_ptr<IntegralEngine::TwoBodyEngine> LibintEngine::coulomb() const {
    return std::make_shared<LibintEngine::TwoBodyEngine>(
      ::libint2::Operator::coulomb,
      this->max_nprim_,
      this->max_l_
    );
  }

}
}
