#include <vector>

#include "KB/KBLiteral.h"

#include "Utils.h"
namespace fovris {


namespace {

using std::vector;
using std::map;
using std::set;

map<KBTerm, vector<unsigned>>
    createSameVarsIndices(const std::vector<KBTerm> &terms) {
    map<KBTerm, vector<unsigned>> indices;

    unsigned i = 0;
    for (auto &t : terms) {
        if (t.isVariable()) {
            auto it = indices.find(t);
            if (it == indices.end()) {
                indices.insert({t, {i}});
            } else {
                it->second.push_back(i);
            }
        }
        i++;
    }

    return indices;
}

std::vector<KBTerm> makeVarSet(const std::vector<KBTerm> &terms) {
    std::set<KBTerm> uniqueVars;
    std::vector<KBTerm> set{};
    for (auto &term : terms) {
        if (term.isVariable() && !Contains(uniqueVars, term)) {
            set.push_back(term);
            uniqueVars.insert(term);
        }
    }
    return set;
}
} // namespace

std::set<KBTerm> KBLiteral::getTermSet() const {
    return {terms.begin(), terms.end()};
}


KBLiteral::KBLiteral(unsigned relId, std::vector<KBTerm> terms)
    : relId(relId)
    , varIndices(createSameVarsIndices(terms))
    , varSet(makeVarSet(terms))
    , terms(std::move(terms)) {}

bool KBLiteral::operator==(const KBLiteral &o) const {

    if (relId != o.relId) {
        return false;
    }
    if (terms != o.terms) {
        return false;
    }

    return true;
}

bool HaveCommonVariables(const KBLiteral &a, const KBLiteral &b) {
    for (auto &entry : a.getVarIndices()) {
        if (Contains(b.getVarIndices(), entry.first)) {
            return true;
        }
    }
    return false;
}

std::vector<unsigned> GetOrderedUniqueVarIndices(const KBLiteral &lit) {

    auto varSet = lit.getVarSet();
    auto varIndicesMap = lit.getVarIndices();

    std::vector<unsigned> indices;

    for (auto var : varSet) {
        auto idx = varIndicesMap[var][0];
        indices.push_back(idx);
    }
    return indices;
}
} // fovris
