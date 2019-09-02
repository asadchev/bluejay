#include "bluejay/libint2/libint2.h"
#include <libint2.hpp>
#include <libint2/config.h>

namespace bluejay {
namespace libint2 {

  using bluejay::integral::GaussianShell;

  void initialize() {
    ::libint2::initialize();
  }

  void LibintEngine::initialize() {
    initialize();
  }

  size_t max_num_primitives(const Basis *b) {
    if (b) return basis::max_num_primitives(*b);
    return 10;
  }

  size_t max_angular_momentum(const Basis *b) {
    if (b) return basis::max_angular_momentum(*b);
    return LIBINT_MAX_AM;
  }

  static ::libint2::Shell shell(const IntegralEngine::Shell &s) {
    const auto &p = std::get<0>(s).data();
    const auto &r = std::get<1>(s);
    size_t np = p.size();
    ::libint2::svector<double> a(np);
    ::libint2::svector<double> c(np);
    for (size_t i = 0; i < np; ++i) {
      a[i] = std::get<0>(p[i]);
      c[i] = std::get<1>(p[i]);
    }
    int L = std::get<0>(s).L();
    return ::libint2::Shell(
      a,
      {
        ::libint2::Shell::Contraction{L, true, c}
      }, // no sp shells yet
      { r[0], r[1], r[2] }
    );
  }

  struct LibintEngine::OneBodyEngine : IntegralEngine::OneBodyEngine {

    typedef IntegralEngine::Shell Shell;

    OneBodyEngine(::libint2::Operator op, size_t max_nprim, int max_l) {
      engine_.reset(new ::libint2::Engine(op, max_nprim, max_l));
    }

    const double* compute(
      const Shell &a,
      const Shell &b) override
    {
      assert(::libint2::initialized());
      // libint is C-major
      engine_->compute(shell(b), shell(a));
      return engine_->results()[0];
    }

    template<class Params>
    void set_params(Params && params) {
      this->engine_->set_params(params);
    }

  private:
    std::unique_ptr<::libint2::Engine> engine_;

  };

  struct LibintEngine::TwoBodyEngine : IntegralEngine::TwoBodyEngine {

    typedef IntegralEngine::Shell Shell;

    TwoBodyEngine(::libint2::Operator op, size_t max_nprim, int max_l) {
      assert(max_nprim);
      assert(max_l);
      engine_.reset(new ::libint2::Engine(op, max_nprim, max_l));
    }

    const double* compute(
      const Shell &a,
      const Shell &b,
      const Shell &c,
      const Shell &d) override
    {
      assert(::libint2::initialized());
      // libint is C-major
      const auto &results = engine_->compute(shell(a), shell(b), shell(c), shell(d));
      return results.at(0);
    }

  private:
    std::unique_ptr<::libint2::Engine> engine_;

  };

  std::shared_ptr<IntegralEngine::OneBodyEngine> LibintEngine::overlap(const Basis *b) const {
    return std::make_shared<LibintEngine::OneBodyEngine>(
      ::libint2::Operator::overlap,
      max_num_primitives(b),
      max_angular_momentum(b)
    );
  }

  std::shared_ptr<IntegralEngine::OneBodyEngine> LibintEngine::kinetic(const Basis *b) const {
    return std::make_shared<LibintEngine::OneBodyEngine>(
      ::libint2::Operator::kinetic,
      max_num_primitives(b),
      max_angular_momentum(b)
    );
  }

  std::shared_ptr<IntegralEngine::OneBodyEngine> LibintEngine::nuclear(const Basis *b) const {
    assert(b);
    auto engine = std::make_shared<LibintEngine::OneBodyEngine>(
      ::libint2::Operator::nuclear,
      max_num_primitives(b),
      max_angular_momentum(b)
    );
    typedef std::array<double,3> double3;
    std::vector< std::pair<double, double3> > q;
    for (const auto& atom : b->atoms()) {
      double3 r = { atom.center[0], atom.center[1], atom.center[2] };
      q.emplace_back(atom.Z, r);
    }
    engine->set_params(q);
    return engine;
  }

  std::shared_ptr<IntegralEngine::TwoBodyEngine> LibintEngine::coulomb(const Basis *b) const {
    return std::make_shared<LibintEngine::TwoBodyEngine>(
      ::libint2::Operator::coulomb,
      max_num_primitives(b),
      max_angular_momentum(b)
    );
  }

}
}
