#ifndef UTILS_H
#define UTILS_H

#include "../Include/tomi.h"

class Utils {
public:
    template<typename E>
    static int find(const Tomi::Vector<int> &ary, const E &x) {
        for (int i = 0; i < ary.size(); i++) {
            if (ary[i] == x)
                return i;
        }
    };

    template<typename E>
    static E del(Tomi::Vector<E> &array, int i) {
        if (i >= 0 && i < array.size()) {

            E tmp = array.back();
            array.pop_back();
            if(i < array.size()) array[i] = tmp;

            return tmp;
        }
       return nullptr;
    };

    // Rotate a long, nice for hashes
    static long rot(unsigned long x, int n);
    static long fold(long x);

    template<typename E>
    static int delVal(Tomi::Vector<E> &array, E i) {
        int counter{};
        for (E item: array) {
            if (item == i) {
                del(array, counter);
                return counter;
            }
            counter++;
        }
        return -1;
    };
};

#endif