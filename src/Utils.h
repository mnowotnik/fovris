#ifndef UTILS_H_GCQCFDEN
#define UTILS_H_GCQCFDEN
#include <algorithm>
#include <iterator>
#include <type_traits>

#include "make_unique.hpp"
namespace fovris {

template <typename T, typename V>
bool Contains(const std::vector<T> &lhs, const V &v) {
    return std::find(lhs.begin(), lhs.end(), v) != lhs.end();
}

template <typename T, typename V> bool Contains(const T &lhs, const V &v) {
    return lhs.find(v) != lhs.end();
}

std::string fileToString(const std::string &path);

std::string fileExtension(const std::string &path);

template <class Iterable, class UnaryPred>
Iterable Filter(const Iterable &iterable, UnaryPred &&pred) {
    Iterable filtered;
    for (auto &p : iterable) {
        if (pred(p)) {
            filtered.insert(p);
        }
    }
    return filtered;
}

template <class Iterable, class UnaryPred>
inline bool AnyOf(const Iterable &iterable, UnaryPred &&pred) {
    return std::any_of(iterable.begin(), iterable.end(),
                       std::forward<UnaryPred>(pred));
}

template <class Iterable, class UnaryPred>
inline bool AllOf(const Iterable &iterable, UnaryPred &&pred) {
    return std::all_of(iterable.begin(), iterable.end(),
                       std::forward<UnaryPred>(pred));
}

template <class Iterable, class UnaryPred>
inline bool NoneOf(const Iterable &iterable, UnaryPred &&pred) {
    return std::none_of(iterable.begin(), iterable.end(),
                        std::forward<UnaryPred>(pred));
}

template <class Iterable, class Val>
inline auto Find(const Iterable &iterable, Val &&val)
    -> decltype(iterable.begin()) {
    return std::find(iterable.begin(), iterable.end(), val);
}

template <class Iterable, class OutputIt, class UnaryPred>
inline OutputIt Transform(Iterable &iterable, OutputIt outputIt,
                          UnaryPred &&pred) {
    return std::transform(iterable.begin(), iterable.end(), outputIt,
                          std::forward<UnaryPred>(pred));
}

template <class Iterable, class VecT, class UnaryPred>
inline typename std::back_insert_iterator<std::vector<VecT>>
    Transform(Iterable &iterable, std::vector<VecT> &vec, UnaryPred &&pred) {
    return std::transform(iterable.begin(), iterable.end(),
                          std::back_inserter(vec),
                          std::forward<UnaryPred>(pred));
}

template <typename T> class RangeIterable {

    T &container;
    using iterator_type = typename std::conditional<std::is_const<T>::value,
                                                    typename T::const_iterator,
                                                    typename T::iterator>::type;
    iterator_type startIt;
    iterator_type endIt;

  public:
    RangeIterable(T &container, size_t start)
        : RangeIterable(container, start, 0) {}
    RangeIterable(T &container, size_t start, size_t end)
        : container(container)
        , startIt(container.begin())
        , endIt(container.begin()) {
        std::advance(startIt, start);
        std::advance(endIt, end);
        if (start > end) {
            start = end;
        }
    }
    iterator_type begin() { return startIt; }
    iterator_type end() { return endIt; }
};


template <typename T, typename... Args>
RangeIterable<T> Range(T &container, Args... args) {
    return RangeIterable<T>(container, std::forward<Args>(args)...);
}


} // fovris
#endif /* end of include guard: UTILS_H_GCQCFDEN */
