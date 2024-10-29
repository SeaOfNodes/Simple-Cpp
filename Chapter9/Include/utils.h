#ifndef UTILS_H
#define UTILS_H
#include <vector>

class Utils {
public:
  template <typename E>
  int find(const std::vector<int>& ary, const E& x);
  template <typename E>
  E del(std::vector<E>& array, int i);
};
#endif