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

  struct Molecule {

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
