#include "bluejay/chem/basis.h"
#include "bluejay/core/json.h"
#include "bluejay/core/system.h"
#include "bluejay/core/string.h"

#include <rapidjson/istreamwrapper.h>
#include <fstream>
#include <iostream>

namespace bluejay {
namespace basis {

  const std::vector<std::string> Shell::Symbols = {
    "s", "p", "d", "f", "g", "h", "i"
  };

  size_t parse_shell_symbol(std::string s) {
    s = string::lowercase(s);
    for (size_t i = 0; i < Shell::Symbols.size(); ++i) {
      if (Shell::Symbols.at(i) == s) return i;
    }
    return -1;
  }

  Shell::Shell(const std::string &s, const std::vector<Primitive> &C)
    : Shell(parse_shell_symbol(s), C)
  {
  }


  std::vector< std::shared_ptr<Shell> > parse_shells(const JsonValue &electron_shells) {
    std::vector< std::shared_ptr<Shell> > shells;
    for (const auto &shell : electron_shells.GetArray()) {
      auto exponents = json::GetArray<std::string>(shell["exponents"]);
      auto angular_momentum = json::GetArray<int>(shell["angular_momentum"]);
      auto coefficients = shell["coefficients"].GetArray();
      for (size_t i = 0; i < angular_momentum.size(); ++i) {
        std::vector<Shell::Primitive> primitives;
        auto ci = json::GetArray<std::string>(coefficients[i]);
        for (size_t j = 0; j < exponents.size(); ++j) {
          double e = std::stod(exponents[j]);
          double c = std::stod(ci[j]);
          primitives.emplace_back(e,c);
        }
        shells.push_back(
          std::make_shared<Shell>(
            angular_momentum[i],
            primitives
          )
        );
      }
    }
    return shells;
  }

  std::unique_ptr<BasisSet> BasisSet::ReadJson(const std::string &path) {
    std::cout << "Reading basis set " << path << std::endl;
    std::ifstream fs(path);
    rapidjson::IStreamWrapper sw(fs);
    JsonDocument json;
    json.ParseStream(sw);
    return BasisSet::ParseJson(json);
  }

  std::unique_ptr<BasisSet> BasisSet::ParseJson(const JsonValue &json) {
    assert(json.IsObject());
    assert(json.HasMember("elements"));
    assert(json["elements"].IsObject());
    std::unique_ptr<BasisSet> basis_set(new BasisSet);
    for (const auto &kv : json["elements"].GetObject()) {
      //std::cout << kv.name.GetString() << std::endl;
      size_t z = std::stoi(kv.name.GetString());
      std::string s = atom::symbol(z);
      auto &atom = basis_set->data_[s];
      auto shells = parse_shells(kv.value["electron_shells"]);
      atom.insert(atom.end(), shells.begin(), shells.end());
    }
    return basis_set;
  }

  BasisSet::BasisSet(const std::string &name, const std::string &path) {
    boost::filesystem::path p = (path != "" ? path : system::libpath());
    auto file = p / "bluejay" / "chem" / "basis" / (name + ".json");
    *this = *BasisSet::ReadJson(file.string());
    // for (const auto &kv : *this) {
    //   std::cout << kv.first << std::endl;
    // }
  }

  const std::vector< std::shared_ptr<Shell> >& BasisSet::operator[](const std::string &name) const {
    auto it = this->find(string::uppercase(name));
    if (it == this->end()) {
      throw Exception("Atom '%s' is not defined in basis set", name);
    }
    return it->second;
  }

  const std::vector<std::shared_ptr<Shell> >& BasisSet::operator[](size_t Z) const {
    std::string s = atom::symbol(Z);
    return this->operator[](s);
  }

}
}
