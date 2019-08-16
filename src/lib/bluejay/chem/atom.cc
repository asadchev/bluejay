#include "bluejay/chem/atom.h"
#include "bluejay/core/string.h"

namespace bluejay {
namespace atom {

  const std::vector<Element> Element::Table = {
     { "" },
     { "H" },
     { "He" },
     { "Li" },
     { "Be" },
     { "B" },
     { "C" },
     { "N" },
     { "O" },
     { "F" },
     { "Ne" },
     { "Na" },
     { "Mg" },
     { "Al" },
     { "Si" },
     { "P" },
     { "S" },
     { "Cl" },
     { "Ar" },
     { "K" },
     { "Ca" },
     { "Sc" },
     { "Ti" },
     { "V" },
     { "Cr" },
     { "Mn" },
     { "Fe" },
     { "Co" },
     { "Ni" },
     { "Cu" },
     { "Zn" },
     { "Ga" },
     { "Ge" },
     { "As" },
     { "Se" },
     { "Br" },
     { "Kr" }
  };

  inline size_t symbol_to_Z(std::string symbol) {
    std::string s = string::uppercase(symbol);
    for (size_t i = 1; i < Element::Table.size(); ++i) {
      if (Element::Table[i].symbol == s) return i;
    }
    throw Exception("Invalid atom symbol '%s'", symbol);
  }

  Atom::Atom(const std::string& s, const Center &r)
    : Atom(symbol_to_Z(s), r)
  {
  }

}
}
