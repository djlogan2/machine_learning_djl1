#ifndef MAPFUNCTIONS_H
#define MAPFUNCTIONS_H

#include <array>

class hashing_func
{
  public:
    unsigned long operator()(const std::array<int, 2> &key) const {
      return key[0] * 100 + key[1];
    };
};

class key_equal_func
{
  public:
    bool operator()(const std::array<int, 2> &a, const std::array<int, 2> &b) const {
      return a[0] == b[0] && a[1] == b[1];
    };
};

#endif // MAPFUNCTIONS_H
