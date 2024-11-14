#include "../Include/utils.h"
#include <iostream>

template <typename E> int Utils::find(const Tomi::Vector<int> &ary, const E &x) {
  for (int i = 0; i < ary.size(); i++) {
    if (ary[i] == x)
      return i;
  }
}
template <typename E> E Utils::del(Tomi::Vector<E> &array, int i) {

  if (i >= 0 && i < array.size()) {

    E tmp = array[i];

    E last = array.back();
    array.pop_back();

    if (i < array.size()) {
      array[i] = last;
    }

    return tmp;
  }

  throw std::out_of_range("Index out of range");
}