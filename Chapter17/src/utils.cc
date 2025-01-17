#include "../Include/utils.h"
#include <iostream>

long Utils::rot(unsigned long x, int n) {
    return (x << n) | (x >> n);
}
// generics only