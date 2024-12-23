#ifndef UTILS_H
#define UTILS_H
#include "../Include/tomi.h"

class Utils {
public:
  template <typename E>
  static int find(const Tomi::Vector<int>& ary, const E& x) {
      for (int i = 0; i < ary.size(); i++) {
          if (ary[i] == x)
              return i;
      }
  };
  template <typename E>
  static E del(Tomi::Vector<E>& array, int i) {
      if (i >= 0 && i < array.size()) {

          E tmp = array.back();
          array.pop_back();
          array[i] = tmp;

          return tmp;
      }

      throw std::out_of_range("Index out of range");
  };
};
#endif