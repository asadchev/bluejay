#include "bluejay/ga/ga.h"
#include <ga.h>

namespace bluejay {
namespace ga {

  void initialize() {
    GA_Initialize();
  }

  void finalize() {
    GA_Terminate();
  }

  struct GlobalArray::Impl {
    int id;
  };

  GlobalArray::GlobalArray(const std::vector<size_t> &shape) {
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
    this->impl_.reset(new Impl{id});
  }

  GlobalArray::~GlobalArray() {
    GA_Destroy(this->id());
  }

  int GlobalArray::id() const {
    return this->impl_->id;
  }

  size_t GlobalArray::ndim() const {
    return GA_Ndim(this->id());
  }

  std::vector<size_t> GlobalArray::shape() const {
    int ndim, type;
    std::vector<int> dims(this->ndim());
    NGA_Inquire(this->id(), &type, &ndim, dims.data());
    //std::reverse(dims.begin(), dims.end());
    return std::vector<size_t>(dims.begin(), dims.end());
  }

  void GlobalArray::set(Slice idx, const void *b) {
    assert(idx.size() == this->ndim());
    assert(b);
    //py::print("lo", idx.lo, "hi", idx.hi, "ld", idx.ld);
    NGA_Put(this->id(), idx.lo.data(), idx.hi.data(), const_cast<void*>(b), idx.ld.data());
  }

  void GlobalArray::get(Slice idx, void *b) const {
    assert(idx.size() == this->ndim());
    assert(b);
    NGA_Get(this->id(), idx.lo.data(), idx.hi.data(), b, idx.ld.data());
  }

}
}
