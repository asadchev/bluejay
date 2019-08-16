#ifndef BLUEJAY_CHEM_BASIS_H
#define BLUEJAY_CHEM_BASIS_H

#include "bluejay/chem/atom.h"
#include "bluejay/core/eigen.h"
#include "bluejay/core/json.h"
#include <memory>
#include <vector>
#include <string>
#include <map>

namespace bluejay {
namespace basis {

  struct Shell;
  struct BasisSet;

  struct Shell {

    static const std::vector<std::string> Symbols;

    typedef std::tuple<double,double> Primitive;

    explicit Shell(const JsonValue &json);

    Shell(const std::string &s, const std::vector<Primitive> &C);

    Shell(size_t L, const std::vector<Primitive> &C)
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

  template<typename T = size_t>
  inline T nbf(const Shell &s) {
    int L = s.L();
    return ((L+1)*(L+2))/2;
  }

  template<typename T = size_t, class ... Args>
  inline T nbf(const Shell &s, const Args& ...  other) {
    return nbf<T>(s)*nbf<T>(other...);
  }

  // inline size_t L(std::initializer_list<Shell> v) {
  //   size_t L = 0;
  //   for (const auto &s : v) {
  //     L += s.L;
  //   }
  //   return L;
  // }

  struct BasisSet {

    typedef basis::Shell Shell;

    static std::unique_ptr<BasisSet> ReadJson(const std::string &path);

    static std::unique_ptr<BasisSet> ParseJson(const JsonValue &json);

    explicit BasisSet(const std::string &name, const std::string &path = "");

    explicit BasisSet(const std::map< std::string, std::vector<std::shared_ptr<Shell> > > &data)
      : data_(data)
    {
    }

    auto begin() const { return data_.begin(); }
    auto end() const { return data_.end(); }
    auto find(const std::string &name) const { return data_.find(name); }

    const std::vector<std::shared_ptr<Shell> >& operator[](size_t Z) const;
    const std::vector<std::shared_ptr<Shell> >& operator[](const std::string &name) const;

    const auto& data() const {
      return data_;
    }

  private:
    BasisSet() {}
    std::map< std::string, std::vector<std::shared_ptr<basis::Shell> > > data_;
    static std::string normalize(const std::string &name);

  };

}
}

namespace bluejay {

  using bluejay::basis::Shell;
  using bluejay::basis::BasisSet;

}

#endif /* BLUEJAY_CHEM_BASIS_H */
