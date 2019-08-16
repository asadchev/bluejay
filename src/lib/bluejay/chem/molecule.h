#ifndef BLUEJAY_CHEM_MOLECULE_H
#define BLUEJAY_CHEM_MOLECULE_H

#include "bluejay/chem/atom.h"
#include "bluejay/chem/basis.h"
#include "bluejay/core/eigen.h"
#include "bluejay/core/json.h"
#include <memory>
#include <vector>
#include <string>
#include <map>

namespace bluejay {
namespace molecule {

  class Molecule;

  struct BasisTuple {
    std::shared_ptr<BasisSet::Shell> shell;
    Atom atom;
    size_t index;
    operator std::tuple<const Shell&, const Vector3<double>&>() const {
      return std::tie(*this->shell, this->atom.center);
    }
    size_t nbf() const {
      return basis::nbf(*this->shell);
    }
  };

  struct MoleculeBasis {

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

    std::vector<size_t> index() const {
      std::vector<size_t> index(data_.size(), 0);
      for (size_t i = 0, idx = 0; i < index.size(); ++i) {
        index[i] = idx;
        idx += this->data_[i].nbf();
      }
      return index;
    }

  private:
    friend class Molecule;
    std::vector<BasisTuple> data_;

  };

  struct Molecule {

    typedef MoleculeBasis Basis;

    Molecule(const std::vector<Atom> &atoms, const BasisSet &basis);

    const auto& basis() const { return basis_; }
    // auto begin() const { return basis_.begin(); }
    // auto end() const { return basis_.end(); }

    size_t nbf() const {
      return this->basis_.nbf();
    }

    // size_t num_shells() const {
    //   return basis_.size();
    // }

    // const BasisSet::Shell& shell(size_t i) const {
    //   return basis_[i].shell;
    // }

    // const Atom::Center& center(size_t i) const {
    //   return basis_[i].center;
    // }

  private:
    Basis basis_;

  };

}
}

namespace bluejay {

  using bluejay::molecule::Molecule;

}


#endif /* BLUEJAY_CHEM_MOLECULE_H */
