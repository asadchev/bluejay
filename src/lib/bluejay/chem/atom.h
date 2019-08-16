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
    Atom(const char* s);
    Atom(const std::string&, const Center &r);
    Atom(size_t Z, const Center &r)
      : center(r), Z(Z)
    {
    }
    static Atom H(const Center &r) {
      return Atom(1, r);
    }
  };

}
}

namespace bluejay {

  using bluejay::atom::Atom;

}

#endif /* BLUEJAY_CHEM_ATOM_H */
