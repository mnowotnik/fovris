#ifndef KNOWLEDGEBASE_H_HRVCJBID
#define KNOWLEDGEBASE_H_HRVCJBID
#include "GroundLiteral.h"
#include "Module.h"
#include "Query.h"
#include "ResultFact.h"
#include "TermMapper.h"

#include "KB/KBGroundLiteral.h"
#include "KB/KBHeadLiteral.h"
#include "KB/KBModule.h"
#include "KB/KBQuery.h"
#include "KB/KBRuleLiteral.h"

#include "Array.h"

#include <vector>

namespace fovris {

class KnowledgeBaseImpl;

class KnowledgeBase {

  public:
    // add new module to KB
    void addModule(const Module &module);

    // add result of evaluation to KB
    void addResultToKB(std::vector<KBTuple> newFacts, unsigned relId);

    const TermMapper &getTermMapper() const;
    KBRelation &getKBRelation(unsigned relId);
    const KBRelation &getKBRelation(unsigned relId) const;
    const std::deque<KBModule> &getKBModules() const;

    KBQuery convertQuery(const Query &query) const;

    ResultFact makeResultFact(Array<KBGroundTerm> tuple, TruthValue value,
                              const std::vector<TermType> &types) const;

    // KB query methods

    Module retrieveModule(const std::string &moduleName) const;

    std::vector<GroundLiteral>
        retrieveRelation(const std::string &moduleName,
                         const std::string &relationName) const;

    std::vector<std::string> listModules() const;
    std::vector<RelationDef> listRelations(const std::string &module) const;
    RelationDef getRelationDef(const std::string &module,
                               const std::string &rel) const;

    KnowledgeBase &operator=(KnowledgeBase &&move);

    KnowledgeBase();
    ~KnowledgeBase();

  private:
    std::unique_ptr<KnowledgeBaseImpl> impl_;
};


} // fovris
#endif /* end of include guard: KNOWLEDGEBASE_H_HRVCJBID */
