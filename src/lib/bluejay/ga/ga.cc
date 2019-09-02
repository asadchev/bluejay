#include "bluejay/ga/ga.h"
#include "bluejay/core/debug.h"
#include <ga.h>
#include <assert.h>

namespace bluejay {
namespace ga {

  void initialize() {
    GA_Initialize();
  }

  void finalize() {
    GA_Terminate();
  }

  struct Patch {
    explicit Patch(size_t ndims, const std::vector< Range<int64_t> > &idx)
      : ndims(ndims)
    {
      for (size_t i = 0; i < idx.size(); ++i) {
        lo.push_back(idx[i].start);
        hi.push_back(idx[i].stop-1);
      }
    }
    const size_t ndims;
    std::vector<int> lo, hi;
  };

  struct GlobalArray::Handle {
    int id;
  };

  GlobalArray::GlobalArray(const std::vector<int64_t> &shape)
    : Array(array::dtype<double>(), shape)
  {
    std::vector<int> dims(shape.begin(), shape.end());
    //std::reverse(dims.begin(), dims.end());
    char name[] = "";
    int id = NGA_Create(
      C_DBL,
      dims.size(),
      dims.data(),
      name,
      NULL
    );
    GA_Zero(id);
    this->handle_.reset(new Handle{id});
  }

  GlobalArray::~GlobalArray() {
    GA_Destroy(this->id());
  }

  int GlobalArray::id() const {
    return this->handle_->id;
  }

  void GlobalArray::get(const std::vector< Range<int64_t> > &idx, const Array::Buffer &buffer) const {
    // Patch p(this->ndim(), idx);
    // void *ptr = const_cast<void*>(buffer.ptr());
    // NGA_Put(this->id(), p.lo.data(), p.hi.data(), ptr, leading_dimensions(buffer).data());
  }

  void GlobalArray::set(const std::vector< Range<int64_t> > &idx, const Array::Buffer &buffer) {
    assert(idx.size() == this->ndim());
    Patch p(this->ndim(), idx);
    debug::print("GlobalArray::set(idx=%s, buffer=%s)", idx, str(buffer));
    auto ld = array::leading_dimensions<double>(buffer);
    std::vector<int> ld_int(ld.begin(), ld.end());
    NGA_Put(this->id(), p.lo.data(), p.hi.data(), (void*)buffer.ptr, ld_int.data());
  }

  // void GlobalArray::set(Slice idx, const void *b) {
  //   assert(idx.size() == this->ndim());
  //   assert(b);
  //   //py::print("lo", idx.lo, "hi", idx.hi, "ld", idx.ld);
  //   NGA_Put(this->id(), idx.lo.data(), idx.hi.data(), const_cast<void*>(b), idx.ld.data());
  // }

  // void GlobalArray::get(Slice idx, void *b) const {
  //   assert(idx.size() == this->ndim());
  //   assert(b);
  //   NGA_Get(this->id(), idx.lo.data(), idx.hi.data(), b, idx.ld.data());
  // }

}
}
