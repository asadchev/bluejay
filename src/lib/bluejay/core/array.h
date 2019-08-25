#ifndef BLUEJAY_ARRAY_H
#define BLUEJAY_ARRAY_H

#include "bluejay/core/eigen.h"
#include "bluejay/core/range.h"
#include "bluejay/core/string.h"
#include "bluejay/core/debug.h"

#include <stddef.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <memory>
#include <assert.h>


namespace bluejay {
namespace array {

  template<typename T>
  struct dtype {
    static std::string format() {
      static_assert(std::is_arithmetic<T>::value, "");
      static_assert(std::is_floating_point<T>::value || std::is_integral<T>::value, "");
      const std::string size = std::to_string(sizeof(T));
      if (std::is_floating_point<T>::value) {
        return "f" + size;
      }
      return (std::is_signed<T>::value ? "i" : "u") + size;
    }
  };

  struct Array : std::enable_shared_from_this<Array> {

    struct Buffer;

    auto ptr() {
      return this->shared_from_this();
    }

    virtual ~Array() {}

    std::string dtype() const {
      return dtype_;
    }

    const auto& dimensions() const {
      return this->dimensions_;
    }

    size_t ndim() const {
      return this->dimensions_.size();
    }

    template<typename T>
    void get(const std::vector< Range<int64_t> > &idx, T *b, const std::vector<int64_t> &strides) const {
      this->get(idx, Buffer(b, shape(idx), strides));
    }

    template<typename T>
    void set(const std::vector< Range<int64_t> > &idx, const T *b, const std::vector<int64_t> &strides) {
      //debug::print("Array::set(idx=%s, b=%p)", idx, b);
      this->set(idx, Buffer(const_cast<T*>(b), shape(idx), strides));
    }

    template<typename T, int N>
    void set(const std::vector< Range<int64_t> > &idx, const Tensor<T,N> &t) {
      // debug::print("Array::set(ndim=%i, idx=%s, t=%s)", this->ndim(), idx, t);
      // debug::print("Array::set: dimensions=%s", this->dimensions_);
      assert(this->ndim() == N);
      const auto &s = strides(t);
      this->set(idx, t.data(), std::vector<int64_t>(s.begin(), s.end()));
    }

    template<typename T, int N>
    void get(const std::vector< Range<int64_t> > &idx, Tensor<T,N> &t) {
      assert(this->ndim() == N);
    }

    virtual void get(const std::vector< Range<int64_t> > &idx, const Buffer &buffer) const = 0;

    virtual void set(const std::vector< Range<int64_t> > &idx, const Buffer &buffer) = 0;

    static auto shape(const std::vector< Range<int64_t> > &idx) {
      std::vector<int64_t> shape;
      for (auto i : idx) {
        shape.push_back(i.stop-i.start);
      }
      return shape;
    }

  protected:

    Array(const Array&) = default;
    Array& operator=(const Array&) = default;

    Array(std::string type, const std::vector<int64_t> &dims)
      : dtype_(type),
        dimensions_(dims)
    {
    }

    template<typename T>
    Array(array::dtype<T> type, const std::vector<int64_t> &dims)
      : Array(type.format(), dims)
    {
    }

    // void get(const std::vector<int64_t> &idx, void *b) const;

    // void set(const std::vector<int64_t> &idx, const void *b);

    std::string dtype_;
    std::vector<int64_t> dimensions_;

  };

  struct Array::Buffer {

    const std::string dtype;
    void* const ptr;
    const std::vector<int64_t> shape;
    const std::vector<int64_t> strides; // in bytes

    Buffer(
      std::string dtype,
      void *ptr,
      const std::vector<int64_t>& shape,
      const std::vector<int64_t>& strides
    )
      : dtype(dtype), ptr(ptr), shape(shape), strides(strides)
    {
    }

    template<typename T>
    Buffer(
      T *ptr,
      const std::vector<int64_t>& shape,
      const std::vector<int64_t>& strides
    )
      : Buffer(array::dtype<T>().format(), ptr, shape, strides)
    {
    }

    template<typename T>
    static std::vector<int64_t> make_strides(const std::vector<int64_t> &shape) {
      size_t bytes = sizeof(T);
      std::vector<int64_t> strides(shape.size());
      for (size_t i = 0; i < shape.size(); ++i) {
        strides[i] = bytes;
        bytes *= shape[i];
      }
      return strides;
    }

  };

  // convert strides to leading dimensions (fortran order)
  template<typename T>
  std::vector<int64_t> leading_dimensions(const Array::Buffer &b) {
    const auto &strides = b.strides;
    std::vector<int64_t> ld(b.strides.size());
    int64_t n = sizeof(T);
    for (size_t i = 0; i < strides.size(); ++i) {
      assert(n <= strides[i]);
      assert(strides[i]%n == 0);
      ld[i] = strides[i]/n;
      n *= strides[i];
    }
    return ld;
  }

}
}

namespace bluejay {

  using bluejay::array::Array;

  inline std::string str(const Array &b) {
    const auto &shape = b.dimensions();
    return (
      std::string("Array(") +
      + "dtype=" + b.dtype()
      + ", shape=[" + string::join(",", shape.begin(), shape.end()) + "]"
      + ")"
    );
  }

  inline std::string str(const Array::Buffer &b) {
    return (
      std::string("Array::Buffer(") +
      + "dtype=" + b.dtype
      + ", ptr=" + str(b.ptr)
      + ", shape=[" + string::join(",", b.shape.begin(), b.shape.end()) + "]"
      + ", strides=[" + string::join(",", b.strides.begin(), b.strides.end()) + "]"
      + ")"
    );
  }

}

#endif /* BLUEJAY_ARRAY_H */
