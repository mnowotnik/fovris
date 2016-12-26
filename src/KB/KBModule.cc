#include "KB/KBModule.h"
#include <set>

namespace fovris {

KBModule::KBModule(std::string name, unsigned id, std::vector<KBRule> rules,
                   std::vector<unsigned> relIds,
                   std::vector<std::reference_wrapper<KBRelation>> relations)
    : name(name)
    , id(id)
    , rules(std::move(rules))
    , relationIds(std::move(relIds))
    , relations(std::move(relations)) {

    std::set<unsigned> idbRelIdsSet;

    for (auto &rule : this->rules) {
        idbRelIdsSet.insert(rule.getHead().getRelId());
    }

    idbRelationIds.reserve(idbRelIdsSet.size());
    idbRelationIds.insert(idbRelationIds.end(), idbRelIdsSet.begin(),
                          idbRelIdsSet.end());
}

unsigned KBModule::factsNumber() const {
    unsigned number = 0;
    for (auto const &rel : relations) {
        number += rel.get().size();
    }
    return number;
}

unsigned KBModule::inconsFactsNumber() const {
    unsigned number = 0;
    for (auto const &rel : relations) {
        number += rel.get().inconsFactsNumber();
    }
    return number;
}
} // fovris
