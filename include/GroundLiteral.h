#ifndef GROUNDLITERAL_H_3KWYY9WG
#define GROUNDLITERAL_H_3KWYY9WG
#include "Literal.h"
namespace fovris {

struct GroundLiteral : Literal {
    GroundLiteral(std::string p, std::vector<Term> terms)
        : Literal(std::move(p), std::move(terms)) {
        validate();
    }

    GroundLiteral(bool n, std::string p, std::vector<Term> terms)
        : Literal(n, std::move(p), std::move(terms)) {
        validate();
    }

    void validate();

    friend std::ostream &operator<<(std::ostream &os, const GroundLiteral &l);
};

} // fovris
#endif /* end of include guard: GROUNDLITERAL_H_3KWYY9WG */
