#ifndef KBMODULE_H_OKG2WQFA
#define KBMODULE_H_OKG2WQFA

#include "KB/KBRelation.h"
#include "KB/KBRule.h"
namespace fovris {

struct KBModule {
    KBModule(std::string name, unsigned id, std::vector<KBRule> rules,
             std::vector<unsigned> relIds,
             std::vector<std::reference_wrapper<KBRelation>> relations);

    const std::string name;
    unsigned id;
    std::vector<KBRule> rules;
    std::vector<unsigned> relationIds;
    std::vector<unsigned> idbRelationIds;
    std::vector<std::reference_wrapper<KBRelation>> relations;
    unsigned factsNumber() const;
    unsigned inconsFactsNumber() const;
};
} // fovris
#endif /* end of include guard: KBMODULE_H_OKG2WQFA */
