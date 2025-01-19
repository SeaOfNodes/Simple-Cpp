#include "../Include/utils.h"
#include <iostream>

long Utils::rot(unsigned long x, int n) {
    return (x << n) | (x >> n);
}
long Utils::fold(long x) {
    return static_cast<int>((x >> 32) ^ x);
}
// generics only