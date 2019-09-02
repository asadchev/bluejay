#include "bluejay/chem/molecule.h"
#include "bluejay/core/forward.h"

namespace bluejay {
namespace molecule {

  Molecule::Molecule(const std::vector<Atom> &atoms, const BasisSet &basis_set)
    : basis_(atoms, basis_set)
  {
  }

}
}
