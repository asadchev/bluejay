#include "bluejay/chem/integral/transform.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/std_tuple.hpp>

namespace bluejay {
namespace integral {
namespace transform {
namespace {

  inline std::string free_indices(const std::string &a, const std::string &b) {
    std::string indices;
    for (auto c : a) {
      if (b.find(c) == std::string::npos) {
        indices += c;
      }
    }
    return indices;
  };

  inline std::string transformed_indices(const std::string &a, const std::string &b) {
    std::string indices;
    for (auto c : a) {
      if (b.find(c) != std::string::npos) {
        indices += c;
      }
    }
    return indices;
  };

  inline std::string unique_set(const std::string &s) {
    std::set<char> u(s.begin(), s.end());
    return std::string(u.begin(), u.end());
  }

  inline std::string duplicate_indices(const std::string &s) {
    std::string duplicate;
    for (auto c : unique_set(s)) {
      if (std::count(s.begin(), s.end(), c) > 1) {
        duplicate += c;
      }
    }
    return duplicate;
  }

}

TransformExpression::TransformExpression(const std::string &expr) {

    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    auto index = ascii::alpha;

    auto intXe = qi::copy('(' >> +index >> '|' >> +index >>')');

    auto grammar = qi::copy(
      +index >> '='
      >> *(+index >> '*')
      >> intXe
      >> *('*' >> +index)
    );

    std::string bra,ket;
    std::vector<std::string> args;

    { // parse expression
      auto it = expr.begin();
      std::string arg0;
      auto result = std::tie(arg0, args, bra, ket, args);
      bool ok = qi::phrase_parse(it, expr.end(), grammar, ascii::space, result);
      //std::cout << std::get<0>(expression) << std::endl;
      Assert(
        (ok && (it == expr.end())),
        "Transform expression '%s' failed to parse", expr
      );
      args.insert(args.begin(), arg0);
    }

    std::string braket = bra+ket;
    Assert(
      (bra.size() == ket.size()) && (braket.size() == 2 || braket.size() == 4),
      "Transform '%s': invalid integral (%s|%s)", expr, bra, ket
    );

    std::string rhs;
    for (size_t k = 1; k < args.size(); ++k) {

      std::string free = free_indices(args[k], bra+ket);
      std::string transformed = transformed_indices(args[k], bra+ket);

      Assert(
        !(transformed.empty() || free.empty()),
        "Transform '%s': argument %i (%s) is not a valid '(%s|%s)' transform",
        expr, k, args[k], bra, ket
      );

      rhs += free;
      rhs += transformed;

    }

    {
      auto duplicate = unique_set(
        duplicate_indices(rhs) +
        duplicate_indices(bra+ket) +
        duplicate_indices(args.at(0))
      );
      Assert(
        duplicate.empty(),
        "Transform '%s': duplicate indices { %s }", expr, duplicate
      );
    }

    {
      const auto &r = free_indices(rhs, bra+ket) + free_indices(bra+ket, rhs);
      const auto &l = args.at(0);
      auto sorted = [](auto r) {
        std::sort(r.begin(), r.end());
        return r;
      };
      Assert(
        sorted(r) == sorted(l),
        "Transform '%s': lhs ('%s') is not equivalent to rhs ('%s')", expr, args.at(0), r
      );
    }

    this->str_ = expr;
    this->args_ = args;
    this->bra_ = bra;
    this->ket_ = ket;

  }

  template<size_t N>
  std::array<size_t,N> TransformExpression::braket() const {
    std::vector< std::tuple<char,size_t> > indices;
    for (char c : (this->bra() + this->ket())) {
      size_t idx = indices.size();
      indices.emplace_back(c,idx);
    }
    Assert(indices.size() == N, "Incorrect integral type, expected %i-center type", (int)N);
    std::sort(indices.begin(), indices.end());
    std::array<size_t,N> type;
    for (size_t i = 0; i < N; ++i) {
      type[i] = std::get<1>(indices[i]);
    }
    return type;
  }

  template
  std::array<size_t,4> TransformExpression::braket() const;

}
}
}
