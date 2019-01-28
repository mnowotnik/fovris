#ifndef PRINT_H_A8EOB9FP
#define PRINT_H_A8EOB9FP
#include <sstream>
#include <utility>

namespace fovris {

struct Identity {
    template <typename U>
    constexpr auto operator()(U &&v) const noexcept
        -> decltype(std::forward<U>(v)) {
        return std::forward<U>(v);
    }
};

template <class Iterable, class Function = Identity>
std::string printIterable(const Iterable &iterable, std::string delim,
                          Function mapper = {}) {
    std::stringstream ss;
    size_t i = 0;
    for (auto &el : iterable) {
        if (i > 0) {
            ss << delim;
        }
        ss << mapper(el);
    }
    return ss.str();
}


template <typename... Args> std::string print(Args &&... args) {
    std::stringstream ss;

    using expander = int[];
    (void)expander{(ss << std::forward<Args>(args), void(), 0)...};

    return ss.str();
}
} // fovris
#endif /* end of include guard: PRINT_H_A8EOB9FP */
