#ifndef ARRAY_H_WNUZDP8G
#define ARRAY_H_WNUZDP8G
#include "make_unique.hpp"

#include <stdexcept>
#include <vector>
#include <initializer_list>

namespace fovris {
template <typename T> class Array {
  public:
    typedef T *iterator;
    typedef const T *const_iterator;
    typedef size_t size_type;

  private:
    size_type size_;
    std::unique_ptr<T[]> store_;

    void swap(Array &o) noexcept {
        store_.swap(o.store_);
        std::swap(o.size_, size_);
    }

  public:
    explicit Array(size_type s) : size_(s), store_(std::make_unique<T[]>(s)) {}
    explicit Array(const typename std::vector<T> &vec)
        : Array(vec.begin(), vec.end()) {}

    Array(std::unique_ptr<T[]> arr, size_type s)
        : size_(s), store_(store_(std::move(arr))) {}

    template <typename InputIt>
    Array(InputIt first, InputIt last)
        : size_(last - first), store_(std::make_unique<T[]>(size_)) {
        std::copy(first, last, begin());
    }

    Array(const Array &copy)
        : size_(copy.size_), store_(std::make_unique<T[]>(copy.size_)) {
        std::copy(copy.begin(), copy.end(), begin());
    }

    Array(std::initializer_list<T> elems)
        : size_(elems.size()), store_(std::make_unique<T[]>(elems.size())) {
        std::copy(elems.begin(), elems.end(), begin());
    }

    Array(Array &&move) = default;

    Array &operator=(Array o) {
        swap(o);
        return *this;
    }

    size_type length() const { return size_; }
    size_type size() const { return size_; }

    const T &at(size_type i) const {
        if (i >= size_) {
            throw std::out_of_range{"Array index out of range!"};
        }
        return store_[i];
    }

    T &at(size_type i) {
        if (i >= size_) {
            throw std::out_of_range{"Array index out of range!"};
        }
        return store_[i];
    }

    T &operator[](size_type i) { return store_[i]; }

    T &front() { return store_[0]; }

    T &back() { return store_[size_]; }

    const T &front() const { return store_[0]; }

    const T &back() const { return store_[size_]; }

    const T &operator[](size_type i) const { return store_[i]; }

    bool operator==(const Array &o) const {
        if (o.size_ != size_) {
            return false;
        }

        for (size_type i = 0; i < size_; ++i) {
            if (o.store_[i] != store_[i]) {
                return false;
            }
        }
        return true;
    }

    bool operator!=(const Array &o) const { return !operator==(o); }

    bool operator<(const Array &o) const {
        if (size_ < o.size_) {
            return true;
        } else if (size_ > o.size_) {
            return false;
        }

        for (size_type i = 0; i < size_; i++) {
            if (!(store_[i] < o.store_[i])) {
                return false;
            }
        }

        return true;
    }

    iterator begin() { return &store_[0]; }

    const_iterator begin() const { return &store_[0]; }

    iterator end() { return &store_[size_]; }

    const_iterator end() const { return &store_[size_]; }
};
} // fovris
#endif /* end of include guard: ARRAY_H_WNUZDP8G */
