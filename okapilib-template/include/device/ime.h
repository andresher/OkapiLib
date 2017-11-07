#ifndef OKAPI_IME
#define OKAPI_IME

#include <API.h>
#include "device/rotarySensor.h"

namespace okapi {
  class IME : public RotarySensor {
  public:
    explicit constexpr IME(const unsigned int iindex):
      index(iindex),
      reversed(1),
      val(0) {}

    explicit constexpr IME(const unsigned int iindex, const bool ireversed):
        index(iindex),
        reversed(ireversed ? -1 : 1),
        val(0) {}

    int get() override { imeGet(index, &val); return reversed * val; }
    void reset() override { imeReset(index); }
  private:
    unsigned int index;
    const int reversed;
    int val;
  };
  
  inline namespace literals {
    constexpr IME operator"" _ime(const unsigned long long int p) { return IME(static_cast<unsigned int>(p), false); }
    constexpr IME operator"" _rime(const unsigned long long int p) { return IME(static_cast<unsigned int>(p), true); }
  }
}

#endif /* end of include guard: OKAPI_IME */