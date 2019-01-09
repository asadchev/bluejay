#ifndef PYJAY_GA_H
#define PYJAY_GA_H

#include "bluejay/core/eigen.h"
#include <vector>
#include <memory>

namespace bluejay {
namespace ga {

  void initialize();

  void finalize();

  struct GlobalArray {

    struct Slice {
      explicit Slice(const std::vector<int64_t> &idx) {
        for (size_t i = 0; i < idx.size(); ++i) {
          lo.push_back(idx[i]);
          hi.push_back(idx[i]);
          ld.push_back(1);
        }
      }
      explicit Slice(
        const std::vector< range<int64_t> > &idx,
        const std::vector<int64_t> &ld)
      {
        for (size_t i = 0; i < idx.size(); ++i) {
          lo.push_back(idx[i].start);
          hi.push_back(idx[i].stop-1);
        }
        this->ld.assign(ld.begin(), ld.end());
      }
      size_t size() const {
        return lo.size();
      }
      std::vector<int> lo, hi, ld;
    };


    explicit GlobalArray(const std::vector<size_t> &shape);

    ~GlobalArray();

    int id() const;

    size_t ndim() const;

    std::vector<size_t> shape() const;

    double get(const std::vector<int64_t> &idx) const {
      double b;
      get(Slice(idx), &b);
      return b;
    }

    void get(
      const std::vector< range<int64_t> > &idx,
      void *b,
      const std::vector<int64_t> &ld) const
    {
      get(Slice(idx, ld), b);
    }


    void set(const std::vector<int64_t> &idx, double b) {
      set(Slice(idx), &b);
    }

    void set(const std::vector< range<int64_t> > &idx, void *b, const std::vector<int64_t> &ld) {
      set(Slice(idx, ld), b);
    }

  private:
    void set(Slice idx, const void *b);
    void get(Slice idx, void *b) const;

  private:
    struct Impl;
    std::unique_ptr<Impl> impl_;

  };


}
}

#endif /* PYJAY_GA_H */
