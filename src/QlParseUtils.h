#ifndef QLPARSEUTILS_H_12NVYFSL
#define QLPARSEUTILS_H_12NVYFSL
#include <map>

#include "make_unique.hpp"
#include "Term.h"

namespace fovris {


struct PValueType {
    std::unique_ptr<std::string> alias;
    const TermType valueType = TermType::Var;

    PValueType(PValueType &&move)
        : alias(std::move(move.alias)), valueType(move.valueType) {}

    PValueType(const PValueType &copy) : valueType(copy.valueType) {
        if (copy.alias) {
            alias = std::make_unique<std::string>(*copy.alias);
        }
    }

    PValueType(std::string alias)
        : alias(std::make_unique<std::string>(std::move(alias))) {}

    PValueType(TermType vt) : alias(nullptr), valueType(vt) {}

    bool aliased() const { return alias.get() != nullptr; }
};

struct PRelationDef {
    PRelationDef(std::string ps, std::vector<PValueType> &&t)
        : predicateSymbol(std::move(ps)), types(std::move(t)) {}
    const std::string predicateSymbol;
    const std::vector<PValueType> types;
};

using Domain = std::pair<std::string, TermType>;
using DomainsMap = std::map<std::string, TermType>;

struct ResolveInfo {
    std::unique_ptr<std::vector<RelationDef>> relations;
    std::string errMsg;
    ResolveInfo(decltype(relations) r, std::string errMsg = {})
        : relations(std::move(r)), errMsg(errMsg) {}
};

ResolveInfo resolveDomains(
    const DomainsMap &domains,
    const std::vector<PRelationDef> &p_relations) {
    auto relations = std::make_unique<std::vector<RelationDef>>();

    if (p_relations.empty()) {
        return {std::move(relations)};
    }

    for (auto &p_r : p_relations) {
        std::vector<TermType> types;

        for (auto &p_t : p_r.types) {
            if (!p_t.aliased()) {
                types.push_back(p_t.valueType);
            } else if (domains.find(*p_t.alias) != domains.end()) {
                types.push_back(domains.at(*p_t.alias));
            } else {
                return {std::move(relations),
                        print("Unrecognized domain: '", *p_t.alias, "'")};
            }
        }
        relations->push_back(
            {std::move(p_r.predicateSymbol), std::move(types)});
    }
    return {std::move(relations)};
}

template <typename X>
void moveAppendVectors(std::vector<X> &src, std::vector<X> &dst) {
    if (dst.empty()) {
        dst = std::move(src);
    } else {
        dst.reserve(dst.size() + src.size());
        std::move(std::begin(src), std::end(src), std::back_inserter(dst));
        src.clear();
    }
}

struct ModuleBody {
    std::vector<RelationDef> relations;
    std::vector<Rule> rules;
    std::vector<GroundLiteral> facts;
};
} // fovris
#endif /* end of include guard: QLPARSEUTILS_H_12NVYFSL */
