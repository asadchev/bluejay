#ifndef BLUEJAY_GA_H
#define BLUEJAY_GA_H

#include "bluejay/core/array.h"
#include <vector>
#include <memory>

namespace bluejay {
namespace ga {

  void initialize();

  void finalize();

  struct GlobalArray : bluejay::Array {

    explicit GlobalArray(const std::vector<int64_t> &shape);

    ~GlobalArray();

    int id() const;

    void get(const std::vector< Range<int64_t> > &idx, const Array::Buffer &buffer) const override;

    void set(const std::vector< Range<int64_t> > &idx, const Array::Buffer &buffer) override;

  private:
    struct Handle;
    std::unique_ptr<Handle> handle_;

  };


}
}

#endif /* BLUEJAY_GA_H */
