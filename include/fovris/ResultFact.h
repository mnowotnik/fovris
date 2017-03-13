#ifndef RESULTFACT_H_Z6TDOW51
#define RESULTFACT_H_Z6TDOW51
#include "Array.h"
#include "Term.h"
#include "TruthValue.h"
namespace fovris {
struct ResultFact {
    const Array<Term> terms;
    const TruthValue value;

    ResultFact(Array<Term> terms, TruthValue value)
        : terms(std::move(terms)), value(value) {}

    typedef decltype(terms)::size_type size_type;

    bool isFalse() { return value == TruthValue::False; }
    bool isInconsistent() { return value == TruthValue::Incons; }
    bool isTrue() { return value == TruthValue::True; }

    const Term &operator[](size_type i) const { return terms[i]; }

    const Term &front() const { return terms.front(); }

    const Term &back() const { return terms.back(); }

    size_type size() const { return terms.size(); }

    bool operator<(const ResultFact &o) const {
        if (o.value != value) {
            return value < o.value;
        }
        return terms < o.terms;
    }

    bool operator==(const ResultFact &o) const {
        if (o.value != value) {
            return false;
        }
        return terms == o.terms;
    }

    decltype(terms)::const_iterator begin() const { return terms.begin(); }
    decltype(terms)::const_iterator end() const { return terms.end(); }

    friend std::ostream &operator<<(std::ostream &os, const ResultFact &q);
};
} // fovris;
#endif /* end of include guard: RESULTFACT_H_Z6TDOW51 */
