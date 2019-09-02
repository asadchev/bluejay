#ifndef BLUEJAY_CHEM_BASIS_H
#define BLUEJAY_CHEM_BASIS_H

#include "bluejay/chem/atom.h"
#include "bluejay/core/eigen.h"
#include "bluejay/core/json.h"
#include "bluejay/core/range.h"

#include <memory>
#include <vector>
#include <string>
#include <map>

namespace bluejay {
namespace basis {

  struct GaussianShell {

    static const std::vector<std::string> Symbols;

    typedef std::tuple<double,double> Primitive;

    explicit GaussianShell(const JsonValue &json);

    GaussianShell(const std::string &s, const std::vector<Primitive> &C);

    GaussianShell(size_t L, const std::vector<Primitive> &C)
      : L_(L), data_(C)
    {
    }

    Primitive operator[](size_t i) const {
      return data_[i];
    }

    auto begin() const { return data_.begin(); }
    auto end() const { return data_.end(); }

    size_t L() const {
      return L_;
    }

    const std::vector<Primitive>& data() const {
      return data_;
    }

    std::string symbol() const {
      return Symbols.at(L_);
    }

  private:
    const size_t L_;
    const std::vector<Primitive> data_;

  };

  inline size_t nbf(const GaussianShell &s) {
    int L = s.L();
    return (L*2+1); // *(L+2))/2;
  }

  template<typename T = size_t, class ... Args>
  inline T nbf(const GaussianShell &s, const Args& ...  other) {
    return nbf<T>(s)*nbf<T>(other...);
  }

  // inline size_t L(std::initializer_list<GaussianShell> v) {
  //   size_t L = 0;
  //   for (const auto &s : v) {
  //     L += s.L;
  //   }
  //   return L;
  // }

  struct BasisSet {

    typedef GaussianShell Shell;

    static std::unique_ptr<BasisSet> ReadJson(const std::string &path);

    static std::unique_ptr<BasisSet> ParseJson(const JsonValue &json);

    explicit BasisSet(const std::string &name, const std::string &path = "");

    explicit BasisSet(const std::map< std::string, std::vector<std::shared_ptr<Shell> > > &data);

    auto begin() const { return data_.begin(); }
    auto end() const { return data_.end(); }

    auto find(const std::string &name) const {
      return data_.find(normalize(name));
    }

    const std::vector<std::shared_ptr<Shell> >& operator[](size_t Z) const;
    const std::vector<std::shared_ptr<Shell> >& operator[](const std::string &name) const;

    const auto& data() const {
      return data_;
    }

  private:
    BasisSet() {}
    std::map< std::string, std::vector<std::shared_ptr<Shell> > > data_;
    static std::string normalize(const std::string &name);

  };


  struct Basis {

    struct Shell {
      std::shared_ptr<GaussianShell> shell;
      Atom atom;
      const size_t index;
      operator std::tuple<const GaussianShell&, const Vector3<double>&>() const {
        return std::tie(*this->shell, this->atom.center);
      }
      size_t nbf() const {
        return basis::nbf(*this->shell);
      }
      operator Range<size_t>() const {
        return Range<size_t>(index, index+this->nbf());
      }
    };

    Basis(const std::vector<Atom> &atoms, const BasisSet &basis_set);

    const auto& atoms() const {
      return this->atoms_;
    }

    size_t size() const { return data_.size(); }

    auto begin() const { return data_.begin(); }
    auto end() const { return data_.end(); }

    const auto& operator[](size_t idx) const {
      return data_[idx];
    }

    size_t nbf() const {
      size_t n = 0;
      for (const auto &s : this->data_) {
        n += s.nbf();
      }
      return n;
    }

    std::array<size_t,2> offset(size_t a, size_t b) const {
      return {
        this->operator[](a).index,
        this->operator[](b).index
      };
    }

    std::array<size_t,2> extent(size_t a, size_t b) const {
      return {
        this->operator[](a).nbf(),
        this->operator[](b).nbf()
      };
    }

    std::vector<size_t> index() const {
      std::vector<size_t> index(data_.size(), 0);
      for (size_t i = 0; i < index.size(); ++i) {
        index[i] = this->data_[i].index;
      }
      return index;
    }

  private:
    std::vector<Atom> atoms_;
    std::vector<Shell> data_;

  };

  inline size_t nbf(const Basis &b) {
    return b.nbf();
  }

  inline size_t nbf(const Basis::Shell &s) {
    return s.nbf();
  }

  inline size_t max_angular_momentum(const Basis &basis) {
    size_t L = 0;
    for (const auto &s : basis) {
      L = std::max(s.shell->L(), L);
    }
    return L;
  }

  inline size_t max_num_primitives(const Basis &basis) {
    size_t n = 0;
    for (const auto &s : basis) {
      n = std::max(s.shell->data().size(), n);
    }
    return n;
  }


}
}

namespace bluejay {

  using bluejay::basis::BasisSet;
  using bluejay::basis::Basis;

}

#endif /* BLUEJAY_CHEM_BASIS_H */
