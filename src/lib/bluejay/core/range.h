#ifndef BLUEJAY_CORE_RANGE_H
#define BLUEJAY_CORE_RANGE_H

#include <limits>
#include <ostream>

namespace bluejay {

  template<typename T>
  struct Range {
    static const T min = std::numeric_limits<T>::lowest();
    static const T max = std::numeric_limits<T>::max();
    Range(T start, T stop, T step = T(1))
      : start(start), stop(stop), step(step)
    {
    }
    explicit Range(T stop)
      : Range(0, stop)
    {
    }
  public:
    const T start;
    const T stop;
    const T step;
  };

  template<typename T>
  inline std::ostream& operator<<(std::ostream& os, const Range<T> &r) {
    os << "(";
    if (r.start != r.min) os << r.start;
    os << ":";
    if (r.stop != r.max) os << r.stop;
    os << ":" << r.step;
    os << ")";
    return os;
  }

}

#endif // BLUEJAY_CORE_RANGE_H
