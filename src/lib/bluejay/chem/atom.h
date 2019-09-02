#ifndef BLUEJAY_CHEM_ATOM_H
#define BLUEJAY_CHEM_ATOM_H

#include "bluejay/core/eigen.h"
#include "bluejay/core/exception.h"
#include <vector>
#include <string>
#include <map>

namespace bluejay {
namespace atom {

  struct Element {
    const std::string symbol;
    const double weight;
    static const std::vector<Element> Table;
  };

  inline std::string symbol(int Z) {
    if (Z > 0 and Z < (int)Element::Table.size()) {
      return Element::Table.at(Z).symbol;
    }
    throw Exception("Invalid atom Z=%i", Z);
  }

  struct Atom {
    typedef Vector3<double> Center;
    const Center center;
    const size_t Z;
    Atom(const std::string&, const Center &r = {0,0,0});
    Atom(size_t Z, const Center &r = {0,0,0})
      : center(r), Z(Z)
    {
    }
    static Atom H(const Center &r) {
      return Atom(1, r);
    }
  };

  inline size_t num_electrons(const std::vector<Atom> &atoms) {
    size_t n = 0;
    for (const auto &a : atoms) {
      n += a.Z;
    }
    return n;
  }


}
}

namespace bluejay {

  using bluejay::atom::Atom;

}

#endif /* BLUEJAY_CHEM_ATOM_H */
