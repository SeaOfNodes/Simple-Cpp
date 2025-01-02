// Small Standard Lib for the Donsus project(c++23)
#ifndef TOMI_H
#define TOMI_H

#include <cassert>
#include <initializer_list>
#include <iostream>
#include <span>

namespace Tomi {
    template<typename Type>
    class Vector {
    public:
        // its okay from iterator to const_iterator
        // its not okay from const_iterator to iterator
        // (not implicitly convertible)

        using iterator = Type *;
        using const_iterator = const Type *;

        Vector() noexcept
                : capacity(0), current(0), array(nullptr),
                  endPtr(nullptr) {
            /*    array = new Type[capacity];*/
            /*    endPtr = array + current;*/
        };

        Vector(std::initializer_list<Type> init) noexcept
                : capacity(init.size()), current(0) {
            array = new Type[capacity];
            for (auto it = init.begin(); it != init.end(); ++it) {
                push_back(*it);
            }
            endPtr = array + current;
        }

        Vector(std::size_t count, const Type &value) noexcept
                : capacity(count), current(0) {
            array = new Type[capacity];
            for (std::size_t i = 0; i < count; i++) {
                push_back(value);
            }
            endPtr = array + current;
        }

        Vector(std::size_t count) noexcept: capacity(count), current(count) {
            array = new Type[capacity];
            endPtr = array + current;
        }

        Vector(const Vector &other) noexcept
                : capacity(other.capacity), current(other.current),
                  array(new Type[other.capacity]) {
            std::copy(other.array, other.array + other.current, array);
            endPtr = array + current;
        }

        // Move resources instead of deep copies
        Vector(Vector &&other)
                : array(other.array), capacity(other.capacity), current(other.current),
                  endPtr(other.endPtr) {
            other.array = nullptr;
            other.capacity = 0;
            other.current = 0;
            other.endPtr = nullptr;
            endPtr = array + current;
        }

        void swap(Vector &other) {
            std::swap(array, other.array);
            std::swap(endPtr, other.endPtr);
            std::swap(current, other.current);
            std::swap(capacity, other.capacity);
        }

        Vector &operator=(const Vector &other) noexcept {
            if (this != &other) {
                delete[] array;
                capacity = other.capacity;
                array = new Type[capacity];
                current = 0;
                for (auto it = other.begin(); it != other.end(); ++it) {
                    push_back(*it);
                }
                endPtr = array + current;
                return *this;
            }
            return *this;
        }

        Vector &operator=(Vector &&other) noexcept {
            if (this != &other)
                delete[] array;
            capacity = other.capacity;
            array = new Type[capacity];
            current = 0;
            for (auto it = other.begin(); it != other.end(); ++it) {
                push_back(*it);
            }
            delete[] other.array;
            other.array = nullptr;
            other.capacity = 0;
            other.current = 0;
            endPtr = array + current;
            return *this;
        };

        Vector &operator=(std::initializer_list<Type> ilist) noexcept {
            delete[] array;
            array = new Type[capacity];
            for (auto it = ilist.begin(); it != ilist.end(); ++it) {
                push_back(*it);
            }
            endPtr = array + current;
            return *this;
        };

        void shrink(int n) noexcept {
            for (int i = 0; i < n; i++) {
                --endPtr;
            }
        }

        void grow(int n) noexcept {
            for (int i = 0; i < n; i++) {
                ++endPtr;
            }
        }

        ~Vector() noexcept { delete[] array; }

        [[nodiscard]] Type *getData() const noexcept { return array; };

        [[nodiscard]] Type &at(size_t pos) const {
            if (pos <= current) {
                return array[pos];
            }
            throw std::out_of_range("Out of bounds");
        };

        // No bounds checking is preformed
        [[nodiscard]] Type &operator[](size_t pos) const noexcept {
            return array[pos];
        };

        [[nodiscard]] iterator begin() const noexcept { return array; }

        [[nodiscard]] iterator end() const noexcept { return endPtr; }

        void push_back(const Type &value) noexcept {
            if (current == capacity) {
                realloc(capacity * 2);
            }
            array[current] = value;
            current++;
            endPtr = array + current;
        };

        void push_back(Type &&value) noexcept {
            if (current == capacity) {
                realloc(capacity * 2);
            }
            array[current] = std::move(value);
            current++;
            endPtr = array + current;
        }

        template<class... Args>
        void emplace_back(Args &&...args) noexcept {
            if (current == capacity) {
                realloc(capacity * 2);
            }
            array[current] = Type(std::forward<Args>(args)...);
            current++;
            endPtr = array + current;
        };

        bool operator==(const Tomi::Vector<Type> &other) const{
            if (size() != other.size()) {
                return false;
            }
            for (size_t i = 0; i < size(); i++) {
                if (array[i] != other[i]) {
                    return false;
                }
            }
            return true;
        }

        template<class... Args>
        iterator emplace(const_iterator pos, Args &&...args) noexcept {
            auto value = type(args...);
            insert(pos, 1, value);
            endPtr = array + current;
        };

        [[nodiscard]] bool empty() const { return current == 0; };

        [[nodiscard]] size_t size() const { return current; };

        [[nodiscard]] size_t get_capacity() const { return capacity; }

        void reserve(std::size_t new_capacity) noexcept {
            if (new_capacity > capacity) {
                realloc(new_capacity);
                return;
            }
            return;
        }

        void resize(size_t count) noexcept {
            // Fix this to affect capacity
            if (count == size())
                return;
            if (current > count) {
                auto diff = current - count;
                shrink(diff);
                current = count;
            } else if (current < count) {
                auto diff = count - current;
                for (unsigned long i = 0; i < diff; i++) {
                    push_back(0);
                    grow(1);
                }
            }
            endPtr = array + current;
        }

        void resize(size_t count, const Type &value) {
            if (count == size())
                return;
            if (current > count) {
                auto diff = current - count;
                shrink(diff);
                current = count;
            } else if (current < count) {
                auto diff = count - current;
                for (unsigned long i = 0; i < diff; i++) {
                    push_back(value);
                    grow(1);
                }
            }
            endPtr = array + current;
        }

        iterator insert(const_iterator pos, const Type &value) noexcept {
            return insert(pos, 1, value);
        };

        iterator insert(const_iterator pos, Type &&value) noexcept {
            return insert(pos, 1, std::move(value));
        };

        iterator insert(const_iterator pos, std::size_t count,
                        const Type &value) noexcept {
            // cast from const (const_iterator) to non-const(iterator)
            auto offset = pos - begin();
            if (count == 0) {
                return begin() + offset;
            }

            if (current + count > capacity) {
                Type *tmp = new Type[2 * capacity];

                std::copy(begin(), begin() + offset, tmp);
                std::copy(begin() + offset, end(), tmp + offset + count);
                delete[] array;
                capacity *= 2;
                array = tmp;
            }

            auto new_pos = begin() + offset;
            std::move_backward(new_pos, end(), end() + count);
            std::fill(new_pos, new_pos + count, value);

            current += count;
            endPtr = array + current;
            // print array
            return new_pos;
        };

        iterator insert(const_iterator pos, std::size_t count,
                        std::initializer_list<Type> ilist) noexcept {
            for (auto it = ilist.begin(); it != ilist.end(); ++it) {
                insert(pos, 1, *it);
                grow(1);
            }
            endPtr = array + current;
        };

        //  Removes the elements in the range [first, last).
        //  Removes the elements in the range [first, last).
        iterator erase(iterator pos) noexcept {
            // TBD
            --current;
            for (iterator it = pos; it < endPtr - 1; ++it) {
                *it = *(it + 1);
            }
            endPtr = array + current;
            return pos;
        }

        iterator erase(const_iterator pos) noexcept {
            // TBD
            --current;
            // cursed shit
            iterator mutablePos = const_cast<iterator>(pos);
            for (iterator it = mutablePos; it <= (endPtr - 1); ++it) {
                *it = *(it + 1);
            }
            endPtr = array + current;
            return pos;
        }

        iterator erase(iterator first, iterator last) noexcept {
            // TBD
            // endPtr = array + current;
            throw std::runtime_error("Not implemented yet");
        }

        iterator erase(const_iterator first, const_iterator last) noexcept {
            // TBD
            // endPtr = array + current;
            throw std::runtime_error("Not implemented yet");;
        }

        void clear() {
            current = 0;
            endPtr = array + current;
        }

        Type &front() noexcept {
            // c.front() is equivalent to *c.begin().
            assert(size() > 0);
            return array[0];
        }

        Type &back() noexcept {
            assert(size() > 0);
            if (size() == 1) {
                return front();
            }
            // Element here points after the last one
            return *(end() - 1);
        }

        std::size_t max_size() noexcept { return capacity; }

        void pop_back() noexcept {
            shrink(1);
            --current;
        }

    private:
        void realloc(std::size_t new_capacity) noexcept {
            if (capacity == 0) {
                capacity = 1;
                array = new Type[capacity];
                return;
            }
            Type *tmp = new Type[new_capacity];
            for (std::size_t i = 0; i < capacity; i++) {
                tmp[i] = array[i];
            }

            delete[] array;
            capacity = new_capacity;
            array = tmp;
        }

        Type *endPtr;
        std::size_t capacity{};
        std::size_t current{};
        Type *array;
    };

    template<size_t SIZE>
// dense-bit array
// non-sparse array of bits
    class BitArray {
    public:
        Tomi::Vector<bool> bits;

        // Initialise the array with a given size
        BitArray() : bits(SIZE, false) {}

        void set(size_t idx) {
            if (idx >= bits.size()) {
                bits.resize(idx + 1, false);
            }
            if (idx < bits.size()) {
                bits[idx] = true;
            } else {
                throw std::runtime_error("Out of bounds: set");
            }
        }

        // Test if a bit at a given index is true
        [[nodiscard]] bool test(size_t index) const {
            if (index < bits.size()) {
                return bits[index];
            } else {
                return false;
            }
        }

        // Reset a bit at a given index to false (or all bits if no index provided)
        void reset(size_t index) {
            if (index < bits.size()) {
                bits[index] = false;
            } else {
                throw std::runtime_error("Out of bounds: reset");
            }
        }

        void reset() {
            std::fill(bits.begin(), bits.end(), false);
        }
    };

    template<typename T, class Container = Vector<T>>
    class Stack {
    public:
        Stack() : stack(Container()) {}

        explicit Stack(const Container &cont) { stack = cont; };

        explicit Stack(Container &&cont) { stack = std::move(cont); };

        Stack(const Stack &other) { stack = other.stack; };

        Stack(Stack &&other) {
            stack = other.stack;
            other.stack = nullptr;
        };

        Stack &operator=(const Stack &other);

        Stack &operator=(Stack &&other);

        T &top() { return stack.back(); }

        bool empty() { return stack.empty(); }

        std::size_t size() const { return stack.size(); }

        void push(const T &value) { stack.push_back(value); }

        void push(T &&value) { stack.push_back(std::move(value)); }

        void pop() { stack.pop_back(); }

        void swap(Stack &other) noexcept { std::swap(stack, other.stack); }

        Container &get_data() noexcept { return stack; }

        template<class... Args>
        decltype(auto) emplace(Args &&...args) {
            return stack.emplace_back(std::forward<Args>(args)...);
        }

        /*  template <class... Args>
          decltype(auto) emplace(Args&&... args) {
            stack
          }*/
    private:
        Container stack;
    };

    namespace detail {
        const unsigned long long TABLE_SIZE = 2;
        static const unsigned long long FNV_OFFSET_BASIS = 0xcbf29ce484222325;
        static const unsigned long long FNV_PRIME = 0x100000001b3;

// implement Fowler–Noll–Vo algorithm
        static unsigned long long fnv_algo(std::span<const std::byte> data) {
            unsigned long long hash = FNV_OFFSET_BASIS;
            for (std::byte byte: data) {
                hash *= FNV_PRIME;
                hash ^= static_cast<unsigned int>(byte);
            }
            return hash;
        }

// keep the node flat
        template<typename K, typename V>
        struct HashNode {
            int long long hash{-1};
            K key;
            V val;
            bool isTombStone{false};

            K getKey() const { return key; }

            V getValue() const { return val; };

            V *getPtrValue() { return &val; }

            void setValue(V value_) { val = value_; }

            void setKey(K key_) { key = key_; }

            void reset() {
                hash = -1;
                isTombStone = false;
            }
        };

        template<typename V>
        struct HashNodeSingle {
            int long long hash{-1};
            V val;
            bool isTombStone{false};

            V getValue() const { return val; }

            V *getPtrValue() { return &val; }

            void setValue(V value_) { val = value_; }

            void reset() {
                hash = -1;
                isTombStone = false;
            }

        };
    } // namespace detail

    template<typename T>
    struct hash {
        unsigned long long operator()(const T &val) {
            throw std::runtime_error("Overload must be provided1");
        }
    };

    template<>
    struct hash<std::string> {
        unsigned long long operator()(const std::string &val) {
            auto *begin = reinterpret_cast<const std::byte *>(&val[0]);
            return detail::fnv_algo({begin, val.size()});
        };
    };

    template<>
    struct hash<int> {
        unsigned long long operator()(const int &val) {
            const unsigned long long A = 0x9e3779b97f4a7c15; // Golden ratio constant
            return val * A;
        };
    };

    template<typename K, typename V>
    class HashMapIterator {
        detail::HashNode<K, V> *current;
        detail::HashNode<K, V> *end;

        void skipEmptyNodes() {
            while (current != end && (current->hash == -1 || current->isTombStone)) {
                ++current;
            }
        }

    public:
        HashMapIterator(detail::HashNode<K, V> *start, detail::HashNode<K, V> *end_)
                : current(start), end(end_) {
            skipEmptyNodes();
        }

        bool operator==(const HashMapIterator &other) const {
            return current == other.current;
        }

        bool operator!=(const HashMapIterator &other) const {
            return current != other.current;
        }

        detail::HashNode<K, V> &operator*() const { return *current; };

        detail::HashNode<K, V> *operator->() const { return current; }

        HashMapIterator &operator++() {
            ++current;
            skipEmptyNodes();
            return *this;
        };
    };
    template <typename V>
    class HashSetIterator {
        detail::HashNodeSingle<V>* current;
        detail::HashNodeSingle<V>* end;

        void skipEmptyNodes() {
            while (current != end && current->hash == -1 || current->isTombStone) {
                ++current;
            }
        }
    public:
        HashSetIterator(detail::HashNodeSingle<V>* start, detail::HashNodeSingle<V>* end_)
                : current(start), end(end_) {
            skipEmptyNodes();
        }

        bool operator==(const HashSetIterator& other) const {
            return current == other.current;
        }

        bool operator!=(const HashSetIterator& other) const {
            return current != other.current;
        }

        detail::HashNodeSingle<V>& operator*() const { return *current; };

        detail::HashNodeSingle<V>* operator->() const { return current; }

        HashSetIterator& operator++() {
            ++current;
            skipEmptyNodes();
            return *this;
        };
    };

    template<typename K, typename V, size_t table_size = detail::TABLE_SIZE,
            typename F = hash<K>>
    class HashMap {
    public:
        HashMap()
                : TableSize(table_size), table(new detail::HashNode<K, V>[TableSize]) {}

        ~HashMap() { delete[] table; }

        HashMap(const HashMap &other)
                : TableSize(other.TableSize), n_elements(other.n_elements), real_size(other.real_size),
                  table(new detail::HashNode<K, V>[other.TableSize]),
                  hashFunc(other.hashFunc) {
            for (size_t i = 0; i < other.TableSize; ++i) {
                table[i] = other.table[i];
            }
        }

        HashMap(HashMap &&other) noexcept
                : TableSize(other.TableSize), n_elements(other.n_elements), real_size(other.real_size),
                  table(other.table), hashFunc(std::move(other.hashFunc)) {
            other.TableSize = 0;
            other.n_elements = 0;
            other.real_size = 0;
            other.table = nullptr;
        }

        HashMap &operator=(const HashMap &other) {
            if (this == &other) {
                return *this;
            }

            delete[] table;

            TableSize = other.TableSize;
            n_elements = other.n_elements;
            real_size = other.real_size;
            hashFunc = other.hashFunc;

            table = new detail::HashNode<K, V>[other.TableSize];
            for (size_t i = 0; i < other.TableSize; ++i) {
                table[i] = other.table[i];
            }

            return *this;
        }

        HashMap &operator=(HashMap &&other) noexcept {
            if (this == &other) {
                return *this;
            }

            delete[] table;

            TableSize = other.TableSize;
            n_elements = other.n_elements;
            real_size = other.real_size;
            hashFunc = std::move(other.hashFunc);
            table = other.table;

            other.TableSize = 0;
            other.n_elements = 0;
            other.real_size = 0;
            other.table = nullptr;

            return *this;
        }

        bool compareKeys(const K &a, const K &b) {
            if constexpr (std::is_pointer_v<K>) {
                return *a == *b;
            } else {
                return a == b;
            }
        }

        V *get(const K &key) {
            unsigned long hashValue = hashFunc(key);
//            if(hashValue == 590625793704138214) {
//                std::cerr << "Here";
//            }

            unsigned long bucketIndex = hashValue % TableSize;
            auto &entry = table[bucketIndex];
            if (entry.hash != -1 && !entry.isTombStone) {
                if (compareKeys(entry.getKey(), key)) {
                    return entry.getPtrValue();
                }
            }

            // Never result in infinite loop because table is never full(invariant)
            while (table[bucketIndex].hash != -1) {
//        if(table[bucketIndex].getKey()->nid == 11) {
//            std::cerr << "Here same key";
//        }
                if (!table[bucketIndex].isTombStone && compareKeys(table[bucketIndex].getKey(), key)) {
                    return table[bucketIndex].getPtrValue();
                }
                // make sure loop will terminate
//      if(bucketIndex + 1 == TableSize) return nullptr;
                bucketIndex = (bucketIndex + 1) % TableSize;
            }
            // not in the table
            return nullptr;
        }
        /*
         * To insert a key–value pair (x,v) into the table
         * (possibly replacing any existing pair with the same key),
         * the insertion algorithm follows the same sequence of cells
         * that would be followed for a search, until finding either
         * an empty cell or a cell whose stored key is x.
         * The new key–value pair is then placed into that cell.[
         */
        // Iterator interface for looping through the hashmap
        HashMapIterator<K, V> begin() { return HashMapIterator<K, V>(table, table + TableSize); }

        HashMapIterator<K, V> end() {
            return HashMapIterator<K, V>(table + TableSize, table + TableSize);
        }

        V *operator[](std::string key) { return get(key); }

        void repopulate() {
            size_t oldTableSize = TableSize;
            TableSize *= 2;
            auto oldTable = table;
            table = new detail::HashNode<K, V>[TableSize];
            n_elements = 0;
            real_size = 0;
            int cont{0};
            for (size_t i = 0; i < oldTableSize; ++i) {
                auto &node = oldTable[i];
                if (node.hash != -1 && !node.isTombStone) {
                    cont++;
                    put(node.key, node.val);
                }
            }
            delete[] oldTable;
        }

        void print() {
            std::cerr << *this;
        }

        void put(const K &key, const V &value) {
            size_t eightyPercentOfTableSize = (TableSize * 8) / 10;
            if (n_elements >= eightyPercentOfTableSize) {
                repopulate();
            }

            unsigned long hashValue = hashFunc(key);
            unsigned long bucketIndex = hashValue % TableSize;
            auto &entry = table[bucketIndex];
            unsigned long originalBucketIndex = bucketIndex;
            // -1 in variant holds if the hash is not set yet
            if (entry.hash == -1) {
                entry.hash = hashValue;
                entry.key = key;
                entry.val = value;
                n_elements++;
                real_size++;

            } else {
                // linear probing loop
                while (table[bucketIndex].hash != -1) {
                    if (table[bucketIndex].isTombStone) {
                        auto &old = table[bucketIndex];
                        old.key = key;
                        old.val = value;
                        old.isTombStone = false;
                        n_elements++;
                        real_size++;
                        return;
                    }
                    if (compareKeys(table[bucketIndex].getKey(), key)) {
                        table[bucketIndex].setValue(value);
                        return;
                    }

                    bucketIndex = (bucketIndex + 1) % TableSize;
                    if (bucketIndex == originalBucketIndex) {
                        // resize
                        // hopefully never get here
                    }
                }
                auto &create_new = table[bucketIndex];
                create_new.hash = hashValue;
                n_elements++;
                real_size++;
                create_new.key = key;
                create_new.val = value;
            }
        }

        // Lazy Deletion(https://en.wikipedia.org/wiki/Lazy_deletion)
        void remove(const K &key) {
            unsigned long hashValue = hashFunc(key);
            unsigned long bucketIndex = hashValue % TableSize;
            auto &entry = table[bucketIndex];
            if (entry.hash == -1) {
                return;
            } else {
                while (table[bucketIndex].hash != -1) {
                    if (compareKeys(table[bucketIndex].getKey(), key)) {
                        auto &old = table[bucketIndex];
                        old.isTombStone = true;
                        n_elements++;
                        real_size--;
                        return;
                    }
                    bucketIndex = (bucketIndex + 1) % TableSize;
                }
            }
        }

        // Clearing the Table - set the hash to -1 and reset the tombstone field
        // O(n)
        void clear() {
            for (size_t i = 0; i < TableSize; ++i) {
                auto &node = table[i];
                node.reset();
            }
            TableSize = Tomi::detail::TABLE_SIZE;
        }

        size_t size() {
            return real_size;
        }

        bool isEmpty() {
            return size() == 0;
        }

    private:
        size_t TableSize;
        size_t n_elements{};
        size_t real_size{};
        detail::HashNode<K, V> *table = nullptr;
        F hashFunc;
    };

    template<typename E, size_t table_size = detail::TABLE_SIZE, typename F = hash<E>>
    class HashSet {
    public:
        HashSet() : TableSize(table_size), table(new detail::HashNodeSingle<E>[TableSize]) {}

        ~HashSet() { delete[] table; }

        HashSet(const HashSet &other) : TableSize(other.TableSize), n_elements(other.n_elements),
                                        real_size(other.real_size),
                                        table(new detail::HashNodeSingle<E>[other.TableSize]),
                                        hashFunc(other.hashFunc) {
            for (size_t i = 0; i < other.TableSize; ++i) {
                table[i] = other.table[i];
            }
        }

        HashSet(HashSet &&other) noexcept
                : TableSize(other.TableSize), n_elements(other.n_elements), real_size(other.real_size),
                  table(other.table), hashFunc(std::move(other.hashFunc)) {
            other.TableSize = 0;
            other.n_elements = 0;
            other.real_size = 0;
            other.table = nullptr;
        }

        HashSetIterator<E> begin() {return HashSetIterator<E>(table, table + TableSize); }
        HashSetIterator<E> end() {return HashSetIterator<E>(table + TableSize, table + TableSize); }

        HashSet &operator=(const HashSet &other) {
            if (this == &other) {
                return *this;
            }

            delete[] table;

            TableSize = other.TableSize;
            n_elements = other.n_elements;
            real_size = other.real_size;
            hashFunc = other.hashFunc;

            table = new detail::HashNodeSingle<E>[other.TableSize];
            for (size_t i = 0; i < other.TableSize; ++i) {
                table[i] = other.table[i];
            }

            return *this;
        }

        HashSet &operator=(HashSet &&other) noexcept {
            if (this == &other) {
                return *this;
            }

            delete[] table;

            TableSize = other.TableSize;
            n_elements = other.n_elements;
            real_size = other.real_size;
            hashFunc = std::move(other.hashFunc);
            table = other.table;

            other.TableSize = 0;
            other.n_elements = 0;
            other.real_size = 0;
            other.table = nullptr;

            return *this;
        }

        bool compareVals(const E &a, const E &b) {
            if constexpr (std::is_pointer_v<E>) {
                return *a == *b;
            } else {
                return a == b;
            }
        }

        E *get(const E &val) {
            unsigned long hashValue = hashFunc(val);
            unsigned long bucketIndex = hashValue % TableSize;
            auto &entry = table[bucketIndex];
            if (entry.hash != -1 && !entry.isTombStone) {
                if (compareVals(entry.getValue(), val)) {
                    return entry.getPtrValue();
                }
            }
            // Never result in infinite loop because table is never full(invariant)
            while (table[bucketIndex].hash != -1) {
//        if(table[bucketIndex].getKey()->nid == 11) {
//            std::cerr << "Here same key";
//        }
                if (!table[bucketIndex].isTombStone && compareVals(table[bucketIndex].getValue(), val)) {
                    return table[bucketIndex].getPtrValue();
                }
                // make sure loop will terminate
//      if(bucketIndex + 1 == TableSize) return nullptr;
                bucketIndex = (bucketIndex + 1) % TableSize;
            }
            // not in the table
            return nullptr;
        }

        void repopulate() {
            size_t oldTableSize = TableSize;
            TableSize *= 2;
            auto oldTable = table;
            table = new detail::HashNodeSingle<E>[TableSize];
            n_elements = 0;
            real_size = 0;
            int cont{0};
            for (size_t i = 0; i < oldTableSize; ++i) {
                auto &node = oldTable[i];
                if (node.hash != -1 && !node.isTombStone) {
                    cont++;
                    put(node.val);
                }
            }
            delete[] oldTable;
        }

        void put(const E &value) {
            size_t eightyPercentOfTableSize = (TableSize * 8) / 10;
            if (n_elements >= eightyPercentOfTableSize) {
                repopulate();
            }
            unsigned long hashValue = hashFunc(value);
            unsigned long bucketIndex = hashValue % TableSize;
            auto &entry = table[bucketIndex];
            unsigned long originalBucketIndex = bucketIndex;
            // -1 in variant holds if the hash is not set yet
            if (entry.hash == -1) {
                entry.hash = hashValue;
                entry.val = value;
                n_elements++;
                real_size++;

            } else {
                // linear probing loop
                while (table[bucketIndex].hash != -1) {
                    if (table[bucketIndex].isTombStone) {
                        auto &old = table[bucketIndex];
                        old.val = value;
                        old.isTombStone = false;
                        n_elements++;
                        real_size++;
                        return;
                    }
                    if (compareVals(table[bucketIndex].getValue(), value)) {
                        table[bucketIndex].setValue(value);
                        return;
                    }

                    bucketIndex = (bucketIndex + 1) % TableSize;
                    if (bucketIndex == originalBucketIndex) {
                        // resize
                        // hopefully never get here
                    }
                }
                auto &create_new = table[bucketIndex];
                create_new.hash = hashValue;
                n_elements++;
                real_size++;
                create_new.val = value;
            }
        }

        // Lazy Deletion(https://en.wikipedia.org/wiki/Lazy_deletion)
        void remove(const E &val) {
            unsigned long hashValue = hashFunc(val);
            unsigned long bucketIndex = hashValue % TableSize;
            auto &entry = table[bucketIndex];
            if (entry.hash == -1) {
                return;
            } else {
                while (table[bucketIndex].hash != -1) {
                    if (compareVals(table[bucketIndex].getValue(), val)) {
                        auto &old = table[bucketIndex];
                        old.isTombStone = true;
                        n_elements++;
                        real_size--;
                        return;
                    }
                    bucketIndex = (bucketIndex + 1) % TableSize;
                }
            }
        }

        // Clearing the Table - set the hash to -1 and reset the tombstone field
        // O(n)
        void clear() {
            for (size_t i = 0; i < TableSize; ++i) {
                auto &node = table[i];
                node.reset();
            }
            TableSize = Tomi::detail::TABLE_SIZE;
        }

        size_t size() {
            return real_size;
        }

        bool isEmpty() {
            return size() == 0;
        }

    private:
        size_t TableSize;
        size_t n_elements{};
        size_t real_size{};
        detail::HashNodeSingle<E> *table = nullptr;
        F hashFunc;
    };
} // namespace Tomi

#endif