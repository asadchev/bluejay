#include "bluejay/chem/molecule.h"
#include "bluejay/core/forward.h"

namespace bluejay {
namespace molecule {

  Molecule::Molecule(const std::vector<Atom> &atoms, const BasisSet &basis) {
    std::map<size_t, std::vector<std::shared_ptr<Shell> > > shells;
    size_t index = 0;
    for (const auto &a : atoms) {
      auto it = shells.find(a.Z);
      if (it == shells.end()) {
        shells.emplace(a.Z, basis[a.Z]);
        it = shells.find(a.Z);
      }
      for (const auto &s : it->second) {
        this->basis_.data_.emplace_back(BasisTuple{s, a, index});
        index += basis::nbf(*s);
      }
    }
  }

}
}
