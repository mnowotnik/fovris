#ifndef RELATIONDEF_H_YIAMYKCW
#define RELATIONDEF_H_YIAMYKCW
#include "Term.h"
namespace fovris {
class RelationDef {
    std::string predicateSymbol;
    std::vector<TermType> types;

  public:
    RelationDef(std::string ps, std::vector<TermType> t)
        : predicateSymbol(std::move(ps)), types(std::move(t)) {}
    const decltype(predicateSymbol) &getPredicateSymbol() const {
        return predicateSymbol;
    }
    const decltype(types) &getTypes() const { return types; }
    friend std::ostream &operator<<(std::ostream &os, const RelationDef &r);
};
} // fovris;
#endif /* end of include guard: RELATIONDEF_H_YIAMYKCW */
