#include "Pred/BinPredicateFunction.h"

#include <string>
#include <utility>
#include <cassert>

#include "make_unique.hpp"
#include "Exceptions.h"
#include "Term.h"
namespace fovris {


namespace {
template <TermType N, typename = void> struct TypeMapper;

template <> struct TypeMapper<TermType::Integer> { typedef int type; };

template <> struct TypeMapper<TermType::Real> { typedef double type; };

template <> struct TypeMapper<TermType::Logic> { typedef TruthValue type; };

template <TermType N>
struct TypeMapper<
    N, typename std::enable_if<N == TermType::String || N == TermType::Date ||
                               N == TermType::DateTime || N == TermType::Id ||
                               N == TermType::Var>::type> {
    typedef std::string type;
};

template <typename T, typename U> struct WiderTypeMapper {
    static_assert(std::is_same<T, U>::value, "Different types");
    typedef T type;
};

template <> struct WiderTypeMapper<double, int> { typedef double type; };

template <> struct WiderTypeMapper<int, double> { typedef double type; };

template <typename Operator, TermType AT, TermType BT,
          typename APT = typename TypeMapper<AT>::type,
          typename BPT = typename TypeMapper<BT>::type,
          typename OpArgType = typename WiderTypeMapper<APT, BPT>::type>
bool dispatch(const Term &a, const Term &b) {
    return Operator::template op<OpArgType>(a.get<APT>(), b.get<BPT>());
}


template <class Operator> class BinPredicateFunction {
  public:
    bool operator()(const Array<Term> &args) {
        assert(args.size() == 2);
        auto &a = args[0];
        auto &b = args[1];

        TermType at = a.getType();
        TermType bt = b.getType();

        if (at == TermType::Integer) {
            if (bt == TermType::Real) {
                return dispatch<Operator, TermType::Integer, TermType::Real>(a,
                                                                             b);
            }

            if (bt == TermType::Integer) {
                return dispatch<Operator, TermType::Integer, TermType::Integer>(
                    a, b);
            }
        }

        if (at == TermType::Real) {
            if (bt == TermType::Real) {
                return dispatch<Operator, TermType::Real, TermType::Real>(a, b);
            }

            if (bt == TermType::Integer) {
                return dispatch<Operator, TermType::Real, TermType::Integer>(a,
                                                                             b);
            }
        }

        assert(at == bt);

        switch (at) {
            case TermType::String:
                return dispatch<Operator, TermType::String, TermType::String>(
                    a, b);
            case TermType::Id:
                return dispatch<Operator, TermType::Id, TermType::Id>(a, b);
            case TermType::Logic:
                return dispatch<Operator, TermType::Logic, TermType::Logic>(a,
                                                                            b);
            case TermType::Date:
                return dispatch<Operator, TermType::Date, TermType::Date>(a, b);
            case TermType::DateTime:
                return dispatch<Operator, TermType::DateTime,
                                TermType::DateTime>(a, b);
            case TermType::Var:
                return dispatch<Operator, TermType::Var, TermType::Var>(a, b);
            default:
                assert(false);
        }

        assert(false);
        return false; // TODO exception
    }
};

struct Less {

    template <typename T> static bool op(const T &t, const T &u) {
        return t < u;
    }
};

struct LessEqual {
    template <typename T> static bool op(const T &t, const T &u) {
        return t <= u;
    }
};


struct Greater {
    template <typename T> static bool op(const T &t, const T &u) {
        return t > u;
    }
};

struct GreaterEqual {
    template <typename T> static bool op(const T &t, const T &u) {
        return t >= u;
    }
};

struct EqualTo {
    template <typename T> static bool op(const T &t, const T &u) {
        return t == u;
    }
};
struct NotEqualTo {
    template <typename T> static bool op(const T &t, const T &u) {
        return t != u;
    }
};
using LessThanPredicate = BinPredicateFunction<Less>;
using LessEqualToPredicate = BinPredicateFunction<LessEqual>;
using GreaterThanPredicate = BinPredicateFunction<Greater>;
using GreaterEqualToPredicate = BinPredicateFunction<GreaterEqual>;
using EqualToPredicate = BinPredicateFunction<EqualTo>;
using NotEqualToPredicate = BinPredicateFunction<NotEqualTo>;
} // namespace

PredicateFunction binDispatch(BuiltinPredicate pred) {
    switch (pred) {
        case BuiltinPredicate::Lt:
            return LessThanPredicate();
        case BuiltinPredicate::Le:
            return LessEqualToPredicate();
        case BuiltinPredicate::Gt:
            return GreaterThanPredicate();
        case BuiltinPredicate::Ge:
            return GreaterEqualToPredicate();
        case BuiltinPredicate::Eq:
            return EqualToPredicate();
        case BuiltinPredicate::Neq:
            return NotEqualToPredicate();
    }
    throw AssertionException("Unsupported BuiltinPredicate");
}
} // fovris
