#include "bluejay/core/array.h"
#include <functional>


namespace bluejay {
namespace array {

  inline auto index_to_ranges(const std::vector<int64_t> &v) {
    std::vector< Range<int64_t> > r;
    r.reserve(v.size());
    for (size_t i = 0; i < v.size(); ++i) {
      r.emplace_back(v[i], v[i]+1);
    }
    return r;
  }

  // void Array::get(const std::vector<int64_t> &idx, void *b) const {
  //   this->get(index_to_ranges(idx), b, std::vector<int64_t>(idx.size(), 1));
  // }

  // void Array::set(const std::vector<int64_t> &idx, const void *b) {
  //   this->set(index_to_ranges(idx), b, std::vector<int64_t>(idx.size(), 1));
  // }

  // void apply(
  //   std::function<void(size_t n, const void*, int64_t, void*, int64_t)> f,
  //   std::vector<size_t> dims,
  //   const void *src,
  //   std::vector<int64_t> src_strides,
  //   void *dst,
  //   std::vector<int64_t> dst_strides)
  // {
  //   size_t N_outer = 0;
  //   for (size_t i = 1; i < dims.size(); ++i) {
  //     N_outer *= dims[i];
  //   }
  //   for (size_t k = 0; k < N_outer; ++k) {
  //     const char *src_k = (char*)src;
  //     char *dst_k = (char*)dst;
  //     for (size_t i = 1; i < dims.size(); ++i) {
  //       size_t j = k/dims[i];
  //       src_k += src_strides.at(j);
  //       dst_k += dst_strides.at(j);
  //     }
  //     f(dims[0], src_k, src_strides[0], dst_k, dst_strides[0]);
  //   }
  // }

}
}
