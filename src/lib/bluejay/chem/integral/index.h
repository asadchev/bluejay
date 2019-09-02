#ifndef BLUEJAY_INTEGRAL_INDEX_H
#define BLUEJAY_INTEGRAL_INDEX_H

#include "bluejay/core/string.h"
#include <array>
#include <set>

namespace bluejay {
namespace integral {

  using IndexPair = std::pair<size_t,size_t>;

  template<typename T = size_t>
  struct IndexList {

    template<typename It>
    IndexList(It begin, It end)
      : data_(begin, end)
    {
    }

    IndexList(std::initializer_list<T> s = {})
      : IndexList(s.begin(), s.end())
    {
    }

    explicit IndexList(std::vector<T> s)
      : IndexList(s.begin(), s.end())
    {
    }

    template<size_t N>
    explicit IndexList(std::array<T,N> s)
      : IndexList(s.begin(), s.end())
    {
    }

    auto begin() const { return data_.begin(); }
    auto end() const { return data_.end(); }

    size_t index(T idx) const {
      auto it = std::find(data_.begin(), data_.end(), idx);
      if (it == data_.end()) {
        return -1;
      }
      return std::distance(data_.begin(), it);
    }

    size_t count(const T &idx) const {
      return std::count(data_.begin(), data_.end(), idx);
    }

    operator std::set<T>() const {
      return std::set<T>(this->begin(), this->end());
    }

    IndexList repeated() const {
      std::set<size_t> s(*this), r;
      for (auto idx : s) {
        if (this->count(idx) > 1) {
          r.insert(idx);
        }
      }
      return IndexList(r.begin(), r.end());
    }

    operator bool() const {
      return !data_.empty();
    }

    void push_back(const T &idx) {
      data_.push_back(idx);
    }

  private:
    std::vector<T> data_;
  };


  template<typename T>
  IndexList<T> operator+(const IndexList<T> &a, const IndexList<T> &b) {
    std::vector<T> v;
    v.insert(v.end(), a.begin(), a.end());
    v.insert(v.end(), b.begin(), b.end());
    return IndexList<T>(v);
  }

  template<typename T>
  IndexList<T> operator-(const IndexList<T> &a, const IndexList<T> &b) {
    std::vector<T> v;
    for (auto idx : a) {
      if (!b.count(idx)) {
        v.push_back(idx);
      }
    }
    return IndexList<T>(v);
  }


  template<size_t N>
  struct IndexShuffle {

    template<typename It>
    static bool validate(It it, It end) {
      std::array<size_t,N> r;
      std::iota(r.begin(), r.end(), 0);
      return std::is_permutation(it, end, r.begin(), r.end());
    }

    template<typename It>
    IndexShuffle(It it, It end) {
      Assert(
        validate(it, end),
        "IndexShuffle<%i>: invalid sequence [ %s ]",
        (int)N, string::join(",", it, end)
      );
      std::copy(it, end, this->data_.begin());
    }

    // explicit IndexShuffle(std::initializer_list<size_t> s)
    //   : IndexShuffle(s.begin(), s.end())
    // {
    // }

    IndexShuffle(std::array<size_t,N> s)
      : IndexShuffle(s.begin(), s.end())
    {
    }

    IndexShuffle operator()(const IndexShuffle &args) const {
      IndexShuffle s;
      s.data_ = this->operator()(args.data_);
      return s;
    }

    template<typename T>
    std::array<T,N> operator()(const std::array<T,N> &args) const {
      return this->apply(args, std::make_index_sequence<N>());
    }

    std::array<size_t,N> operator()(const std::array<size_t,N> &args) const {
      return this->apply(args, std::make_index_sequence<N>());
    }

    auto begin() const { return this->data_.begin(); }
    auto end() const { return this->data_.end(); }

    size_t operator[](size_t idx) const {
      return data_.at(idx);
    }

    IndexShuffle<N> reverse() const {
      IndexShuffle<N> r;
      std::copy(this->data_.rbegin(), this->data_.rend(), r.data_.begin());
      return r;
    }

    // args == ~S(S(args)
    IndexShuffle<N> operator~() const {
      IndexShuffle<N> r;
      for (size_t i = 0; i < N; ++i) {
        r.data_[this->data_[i]] = i;
      }
      return r;
    }

    size_t index(size_t idx) const {
      assert(idx < N);
      for (size_t i = 0; i < N; ++i) {
        if (data_[i] == idx) return i;
      }
      return -1;
    }

  private:

    std::array<size_t,N> data_;

    IndexShuffle() = default;

    template<typename T, typename ... Args, size_t ... Idx>
    auto apply(std::array<T,N> args, std::index_sequence<Idx...>) const {
      return std::array<T,N>{ args[this->operator[](Idx)]... };
    }

  };

}
}

namespace bluejay {

  inline std::string str(const bluejay::integral::IndexPair &idx) {
    return "IndexPair["
      + std::to_string(idx.first) + ","
      + std::to_string(idx.second)
      + "]";
  }

  template<typename T>
  inline std::string str(const bluejay::integral::IndexList<T> &s) {
    return "IndexList[" + string::join(",", s.begin(), s.end()) + "]";
  }

  template<size_t N>
  inline std::string str(const bluejay::integral::IndexShuffle<N> &s) {
    return "IndexShuffle[" + string::join(",", s.begin(), s.end()) + "]";
  }

}

#endif
