#ifndef ARRAYVIEW_H_EFDZYYN3
#define ARRAYVIEW_H_EFDZYYN3

#include <memory>
namespace fovris {

template <typename T> class ArrayView {
  public:
    typedef T *iterator;
    typedef const T *const_iterator;
    typedef size_t size_type;

  private:
    size_type size_;
    T *ptr_;

  public:
    explicit ArrayView(T *view, size_type s) : size_(s), ptr_(view) {}

    size_type length() const { return size_; }
    size_type size() const { return size_; }

    const T &at(size_type i) const throw(std::out_of_range) {
        if (i >= size_) {
            throw std::out_of_range{"ArrayView index out of range!"};
        }
        return ptr_[i];
    }

    T &at(size_type i) throw(std::out_of_range) {
        if (i >= size_) {
            throw std::out_of_range{"ArrayView index out of range!"};
        }
        return ptr_[i];
    }

    T &operator[](size_type i) { return ptr_[i]; }

    T &front() { return ptr_[0]; }

    T &back() { return ptr_[size_]; }

    const T &front() const { return ptr_[0]; }

    const T &back() const { return ptr_[size_]; }

    const T &operator[](size_type i) const { return ptr_[i]; }

    bool operator==(const ArrayView &o) const {
        if (o.size_ != size_) {
            return false;
        }

        for (size_type i = 0; i < size_; ++i) {
            if (o.ptr_[i] != ptr_[i]) {
                return false;
            }
        }
        return true;
    }

    bool operator!=(const ArrayView &o) const { return !operator==(o); }

    bool operator<(const ArrayView &o) const {
        if (size_ < o.size_) {
            return true;
        } else if (size_ > o.size_) {
            return false;
        }

        for (size_type i = 0; i < size_; i++) {
            if (ptr_[i] > o.ptr_[i]) {
                return false;
            }
        }

        return true;
    }

    iterator begin() { return &ptr_[0]; }

    const_iterator begin() const { return &ptr_[0]; }

    iterator end() { return &ptr_[size_]; }

    const_iterator end() const { return &ptr_[size_]; }
};


} // fovris
#endif /* end of include guard: ARRAYVIEW_H_EFDZYYN3 */
