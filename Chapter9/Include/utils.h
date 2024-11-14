#ifndef UTILS_H
#define UTILS_H
#include "../Include/tomi.h"

class Utils {
public:
  template <typename E>
  int find(const Tomi::Vector<int>& ary, const E& x);
  template <typename E>
  E del(Tomi::Vector<E>& array, int i);
};
#endif