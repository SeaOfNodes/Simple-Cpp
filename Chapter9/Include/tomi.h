// Small Standard Lib for the Donsus project(c++23)

#include <cassert>
#include <initializer_list>
#include <iostream>

namespace Tomi {
template <typename Type> class Vector {
public:
  // its okay from iterator to const_iterator
  // its not okay from const_iterator to iterator
  // (not implicitly convertible)

  using iterator = Type *;
  using const_iterator = const Type *;

  Vector() noexcept : capacity(1), current(0) {
    array = new Type[capacity];
    endPtr = array + current;
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

  template <class... Args> void emplace_back(Args &&...args) noexcept {
    if (current == capacity) {
      realloc(capacity * 2);
    }
    array[current] = Type(std::forward<Args>(args)...);
    current++;
    endPtr = array + current;
  };

  template <class... Args>
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
  iterator erase(iterator pos) noexcept {
    // TBD
    --current;
    endPtr = array + current;
    return array + pos;
  };

  iterator erase(const_iterator pos) noexcept {
    // TBD
    endPtr = array + current;
  };

  iterator erase(iterator first, iterator last) noexcept {
    // TBD
    endPtr = array + current;
  };

  iterator erase(const_iterator first, const_iterator last) noexcept {
    // TBD
    endPtr = array + current;
  };

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
    Type *tmp = new Type[2 * new_capacity];
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

template <typename T, class Container = Vector<T>> class Stack {
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

  template <class... Args> decltype(auto) emplace(Args &&...args) {
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
  for (std::byte byte : data) {
    hash *= FNV_PRIME;
    hash ^= static_cast<unsigned int>(byte);
  }
  return hash;
}
// keep the node flat
template <typename K, typename V> struct HashNode {
  unsigned long long hash{};
  K key;
  V val;
  bool isTombStone{false};
  K getKey() const { return key; }
  V getValue() const { return val; };
  const V *getPtrValue() const { return &val; }
  void setValue(V value_) { val = value_; }
  void setKey(K key_) { key = key_; }
};
} // namespace detail

template <typename T> struct hash {
  unsigned long long operator()(const T &val) {
    auto *begin = reinterpret_cast<const std::byte *>(&val[0]);
    return detail::fnv_algo({begin, val.size()});
  }
};

template <typename K, typename V, size_t table_size = detail::TABLE_SIZE,
          typename F = hash<K>>
class HashMap {
public:
  HashMap()
      : TableSize(table_size), table(new detail::HashNode<K, V>[table_size]) {}
  ~HashMap() { delete[] table; }
  const V *get(const K &key) {
    unsigned long hashValue = hashFunc(key) % table_size;
    auto &entry = table[hashValue];
    if (entry.hash != 0 && !entry.isTombStone) {
      if (entry.getKey() == key) {
        return entry.getPtrValue();
      }
    }
    while (table[hashValue].hash != 0 && !table[hashValue].isTombStone) {
      if (table[hashValue].getKey() == key)
        return table[hashValue].getPtrValue();
      hashValue = (hashValue + 1) % table_size;
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
  void repopulate() {
    for (size_t i = 0; i < TableSize;
         ++i) {              // Loop over the indices up to TableSize
      auto &node = table[i]; // Access each node in the table
      if (!node.isTombStone &&
          node.hash != 0) { // Only repopulate non-tombstone, non-empty entries
        put(node.key, node.val);
      }
    }
  }
  void put(const K &key, const V &value) {
    n_elements++;
    size_t seventyFivePercentOfTableSize = TableSize * 8 / 10;
    if (n_elements == seventyFivePercentOfTableSize) {
      TableSize = TableSize * 2;
      delete[] table;
      table = new detail::HashNode<K, V>[TableSize];
      // re add elements that are not dead
      repopulate();
    }
    unsigned long hashValue = hashFunc(key) % table_size;
    auto &entry = table[hashValue];
    unsigned long originalHashValue = hashValue;

    if (entry.hash == 0) {
      entry.hash = hashValue;
      entry.key = key;
      entry.val = value;

    } else {
      // linear probing loop
      while (table[hashValue].hash != 0) {
        if (table[hashValue].isTombStone) {
          auto &old = table[hashValue];
          old.key = key;
          old.val = value;
        }
        if (table[hashValue].getKey() == key) {
          table[hashValue].setValue(value);
          return;
        }

        hashValue = (hashValue + 1) % table_size;
        if (hashValue == originalHashValue) {
          // resize
          // hopefully never get here
        }
      }
      auto &create_new = table[hashValue];
      create_new.hash = hashValue;
      create_new.key = key;
      create_new.val = value;
    }
  }
  // Lazy Deletion(https://en.wikipedia.org/wiki/Lazy_deletion)
  void remove(const K &key) {
    unsigned long hashValue = hashFunc(key) % table_size;
    auto &entry = table[hashValue];

    if (entry.hash == 0) {
      return;
    } else {
      n_elements--;
      while (table[hashValue].hash != 0) {
        if (table[hashValue].getKey() == key) {
          auto &old = table[hashValue];
          old.isTombStone = true;
          return;
        }
        hashValue = (hashValue + 1) % table_size;
      }
    }
  }

private:
  size_t TableSize;
  size_t n_elements{};
  detail::HashNode<K, V> *table;
  F hashFunc;
};
} // namespace Tomi
