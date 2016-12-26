#include "KnowledgeBase.h"
#include "Utils.h"

#include "DataPool.h"
#include "KB/KBRuleDisjunct.h"
#include "Pred/BinPredicateFunction.h"

#include <cassert>
#include <functional>
#include <set>
#include <utility>
namespace fovris {


namespace {

struct VarInfo {
    TermType type;
    bool satisfied;
};

/**
 * @param safe can the variable stand on its own; i.e., is not unsafe or in
 * the head
 */
void calcVarDiff(const std::vector<Term> &terms,
                 const std::vector<TermType> &types,
                 std::map<Term, VarInfo> &varDiff, bool safe) {

    assert(terms.size() == types.size());
    auto termsIt = terms.begin();
    auto typesIt = types.begin();
    for (; termsIt != terms.end(); termsIt++, typesIt++) {
        auto &term = *termsIt;
        auto type = *typesIt;
        if (IsVar(term)) {
            if (!Contains(varDiff, term)) {
                varDiff.insert({term, {type, safe}});
            } else {
                if (safe) {
                    varDiff[term].satisfied = true;
                }
                if (varDiff[term].type != type) {
                    throw InvalidInputException("Type of variable is not the "
                                                "same in every literal. "
                                                "Variable",
                                                term);
                }
            }
        }
    }
}

void checkDomainsTerms(const std::vector<TermType> &colDomains,
                       const std::vector<Term> &terms) {
    if (colDomains.size() != terms.size()) {
        throw InvalidInputException("Wrong arity(", terms.size(),
                                    "). Should be ", colDomains.size(), ".");
    }

    for (size_t i = 0; i < terms.size(); ++i) {
        if (!IsVar(terms[i]) && colDomains[i] != terms[i].getType()) {

            throw InvalidInputException("Wrong constant type (should be ",
                                        colDomains[i], " or variable) at ", i,
                                        " index.");
        }
    }
}

void checkDomainsGroundTerms(const std::vector<TermType> &colDomains,
                             const std::vector<Term> &terms) {
    if (colDomains.size() != terms.size()) {
        throw InvalidInputException("Wrong arity(", terms.size(),
                                    "). Should be ", colDomains.size(), ".");
    }

    for (size_t i = 0; i < terms.size(); ++i) {
        if (colDomains[i] != terms[i].getType() &&
            !CanBe(terms[i], colDomains[i])) {
            throw InvalidInputException("Wrong constant type (should be ",
                                        colDomains[i], ") at ", i, " index.");
        }
    }
}

TermType getTermTypeFromMap(const Term &term,
                            const std::map<Term, VarInfo> &varDiff) {
    auto it = varDiff.find(term);
    if (it == varDiff.end()) {
        throw InvalidInputException("Unknown variable: '", term, "'.");
    }
    return it->second.type;
}

void checkTermsForBuiltinPredicate(const std::vector<Term> &terms,
                                   const std::map<Term, VarInfo> &varDiff) {

    TermType a, b;

    if (terms[0].getType() == TermType::Var) {
        a = getTermTypeFromMap(terms[0], varDiff);
    } else {
        a = terms[0].getType();
    }

    if (terms[1].getType() == TermType::Var) {
        b = getTermTypeFromMap(terms[1], varDiff);
    } else {
        b = terms[1].getType();
    }

    if (a == b) {
        return;
    }

    if (a == TermType::Integer && b == TermType::Real) {
        return;
    }

    if (a == TermType::Real && b == TermType::Integer) {
        return;
    }

    throw InvalidInputException("Incompatible types in predicate(", a, " and  ",
                                b, ").");
}
void checkFunction(const RuleLiteral &literal,
                   const std::map<Term, VarInfo> &varDiff) {
    if (!literal.isBuiltinFunction()) {
        return;
    }
    if (literal.getTerms().size() != 2) {
        throw InvalidInputException("Wrong arity(", literal.getTerms().size(),
                                    "). Should be ", 2, ".");
    }

    try {
        checkTermsForBuiltinPredicate(literal.getTerms(), varDiff);
    } catch (InvalidInputException &e) {
        throw InvalidInputException(e.what(), " In literal: '", literal, "'.");
    }
}

const RelationDef &checkLiteral(
    const std::map<std::string, std::reference_wrapper<const RelationDef>> &
        relDefMap,
    const Literal &literal) {
    auto &pred = literal.getPredicateSymbol();
    auto lIt = relDefMap.find(pred);
    if (lIt == relDefMap.end()) {
        throw InvalidInputException("Relation '", pred, "' not defined.'");
    }

    checkDomainsTerms(lIt->second.get().getTypes(), literal.getTerms());
    return lIt->second;
}


void checkLiteral(
    const std::map<std::string, std::reference_wrapper<const RelationDef>> &
        relDefMap,
    const RuleLiteral &literal) {
    if (!Contains(relDefMap, literal.getPredicateSymbol())) {
        if (!literal.isFunction()) {
            throw InvalidInputException("Unknown relation: '",
                                        literal.getPredicateSymbol(), "'.");
        }
        return;
    }
    checkLiteral(relDefMap, dynamic_cast<const Literal &>(literal));
}

void checkFact(
    const std::map<std::string, std::reference_wrapper<const RelationDef>>
        relDefMap, const GroundLiteral &grndLiteral) {
    auto lIt = relDefMap.find(grndLiteral.getPredicateSymbol());
    if (lIt == relDefMap.end()) {
        throw InvalidInputException(
            "Relation '", grndLiteral.getPredicateSymbol(),
            "' not defined. In literal: '", grndLiteral, "'.");
    }

    try {
        checkDomainsGroundTerms(lIt->second.get().getTypes(),
                                grndLiteral.getTerms());
    } catch (InvalidInputException &e) {
        throw InvalidInputException(e.what(), " In literal: '", grndLiteral,
                                    "'.");
    }
}
}

class KnowledgeBaseImpl {
  public:
    // KnowledgeBase API

    Module retrieveModule(const std::string &moduleName) const;
    std::vector<GroundLiteral>
        retrieveRelation(const std::string &moduleName,
                         const std::string &relationName) const;

    void addModule(const Module &module);
    RelationDef getRelationDef(const std::string &module,
                               const std::string &rel) const;

    const std::deque<KBModule> getKBModules() const;


    ResultFact makeResultFact(const Array<KBGroundTerm> &tuple,
                              TruthValue value,
                              const std::vector<TermType> &types) const;

    Rule convertKBRule(const KBRule &rule, const KBModule &module) const;

    Literal convertKBHeadLiteral(const KBHeadLiteral &literal) const;

    RuleLiteral convertKBRuleLiteral(const KBRuleLiteral &literal,
                                     const KBModule &module) const;

    GroundLiteral
        retrieveGroundLiteral(const KBTuple &tuple, const std::string &relName,
                              const std::vector<TermType> &types) const;

    GroundLiteral retrieveGroundLiteral(unsigned relId,
                                        const KBTuple &tuple) const;

    KBQuery convertQuery(const Query &query) const;
    // private

    unsigned queryModuleId(const std::string &module) const;
    unsigned queryRelationId(unsigned modId, const std::string &rel) const;
    TermType queryTermType(unsigned relationId, unsigned idx) const;
    std::vector<GroundLiteral> retrieveRelation(unsigned relId) const;

    unsigned addRelation(unsigned modId, const RelationDef &def);
    void addFact(unsigned modId, const GroundLiteral &fact);

    // convert api to kb
    KBRule convertRule(unsigned modId, const Rule &rule);

    KBHeadLiteral convertLiteral(unsigned modId, const Literal &literal);

    KBGroundLiteral convertGroundLiteral(unsigned modId,
                                         const GroundLiteral &literal);

    KBRuleLiteral convertRuleLiteral(unsigned modId,
                                     const RuleLiteral &literal);


    KBRuleFunction
        convertRuleFunction(const RuleLiteral &literal,
                            const std::map<std::string, TermType> &typeMap);
    KBTerm convertTerm(const Term &term);

    KBGroundTerm convertGroundTerm(const Term &term);

    void validateModule(const Module &module) const;


    void checkRuleLiteral(
        const std::map<std::string, std::reference_wrapper<const RelationDef>> &
            relDefMap,
        const RuleLiteral &rl, std::map<Term, VarInfo> &varsDiff) const;

    void checkRule(
        const std::map<std::string, std::reference_wrapper<const RelationDef>> &
            relDefMap,
        const Rule &rule) const;

    std::deque<KBRelation> relations_;
    std::deque<KBModule> modules_;

    DataPool<std::string> moduleNamePool_;
    DataPool<std::pair<unsigned, std::string>> relSymPool_;

    TermMapper termMapper_;

    std::map<std::string, TermType> makeTypeMap(const RuleDisjunct &disjunct,
                                                unsigned modId) const;
};

const TermMapper &KnowledgeBase::getTermMapper() const {
    return impl_->termMapper_;
}

KBRelation &KnowledgeBase::getKBRelation(unsigned relId) {
    return impl_->relations_[relId];
}

const KBRelation &KnowledgeBase::getKBRelation(unsigned relId) const {
    return impl_->relations_[relId];
}

const std::deque<KBModule> &KnowledgeBase::getKBModules() const {
    return impl_->modules_;
}

KnowledgeBase::~KnowledgeBase() {}
KnowledgeBase::KnowledgeBase() : impl_(std::make_unique<KnowledgeBaseImpl>()) {}

KnowledgeBase &KnowledgeBase::operator=(KnowledgeBase &&move) {
    impl_ = std::move(move.impl_);
    return *this;
}

void KnowledgeBase::addModule(const Module &module) {
    impl_->addModule(module);
}

const std::deque<KBModule> KnowledgeBaseImpl::getKBModules() const {
    return modules_;
}

void KnowledgeBaseImpl::checkRuleLiteral(
    const std::map<std::string, std::reference_wrapper<const RelationDef>> &
        relDefMap,
    const RuleLiteral &rl, std::map<Term, VarInfo> &varsDiff) const {

    if (!rl.isExternal()) {
        if (rl.hasConstraint()) {
            throw InvalidInputException(
                "'in' operator can only be defined for external literal: '", rl,
                "'.");
        }
        try {
            checkLiteral(relDefMap, rl);
            if (!rl.isFunction()) {
                calcVarDiff(rl.getTerms(),
                            relDefMap.find(rl.getPredicateSymbol())
                                ->second.get()
                                .getTypes(),
                            varsDiff, true);
                return;
            }

        } catch (InvalidInputException &e) {
            throw InvalidInputException(e.what(), " In literal: '", rl, "'.");
        }
        return;
    }

    unsigned modId, relId;

    try {
        modId = moduleNamePool_.queryId(*rl.getExternalModule());
    } catch (std::out_of_range &e) {
        throw InvalidInputException("Module '", *rl.getExternalModule(),
                                    "' not found: '", rl, "'.");
    }


    try {
        relId = relSymPool_.queryId({modId, rl.getPredicateSymbol()});
    } catch (std::out_of_range &e) {
        throw InvalidInputException("Relation('", rl.getPredicateSymbol(),
                                    "')in external literal not found: '", rl,
                                    "'.");
    }

    try {
        checkDomainsTerms(relations_[relId].colDomains, rl.getTerms());
    } catch (InvalidInputException &e) {
        throw InvalidInputException(e.what(), " In literal: '", rl, "'.");
    }

    bool satisfy = true;
    if (rl.isUnsafe()) {
        satisfy = false;
    }
    calcVarDiff(rl.getTerms(), relations_[relId].colDomains, varsDiff, satisfy);
}

std::string unsatisfiedVars(const std::map<Term, VarInfo> &varsDiff) {
    using Pair = std::remove_reference<decltype(varsDiff)>::type::value_type;
    return printIterable(
        Filter(varsDiff, [](const Pair &p) { return !p.second.satisfied; }),
        ",", [](const Pair &p) { return p.first; });
}

void KnowledgeBaseImpl::checkRule(
    const std::map<std::string, std::reference_wrapper<const RelationDef>> &
        relDefMap,
    const Rule &rule) const {

    std::map<Term, VarInfo> varsDiff;

    auto typesIt = relDefMap.find(rule.getHead().getPredicateSymbol());
    if (typesIt == relDefMap.end()) {
        throw InvalidInputException("No such relation '",
                                    rule.getHead().getPredicateSymbol(),
                                    "' defined in the module.");
    }

    calcVarDiff(rule.getHead().getTerms(), typesIt->second.get().getTypes(),
                varsDiff, false);

    try {
        checkLiteral(relDefMap, rule.getHead());
    } catch (InvalidInputException &e) {
        throw InvalidInputException(e.what(), " In literal: '", rule.getHead(),
                                    "'. In the head of rule: ", rule, ".");
    }

    for (auto &disjunct : rule.getDisjuncts()) {
        for (auto &rl : disjunct.getBody()) {
            try {
                checkRuleLiteral(relDefMap, rl, varsDiff);
            } catch (InvalidInputException &e) {
                throw InvalidInputException(e.what(), " In rule: ", rule, ".");
            }
        }

        for (auto &rl : disjunct.getBody()) {
            try {
                checkFunction(rl, varsDiff);
            } catch (InvalidInputException &e) {
                throw InvalidInputException(e.what(), " In rule: ", rule, ".");
            }
        }

        bool allVarsSatisfied =
            AllOf(varsDiff, [](const std::pair<Term, VarInfo> &p) {
                return p.second.satisfied;
            });

        if (!allVarsSatisfied) {
            throw InvalidInputException("Not all variables (",
                                        unsatisfiedVars(varsDiff),
                                        ") are covered in rule: ", rule, ".");
        }
    }
}

void KnowledgeBaseImpl::addModule(const Module &module) {
    validateModule(module);

    auto modId = moduleNamePool_.insert(module.getName()).first;

    std::vector<unsigned> relationIds;
    std::vector<std::reference_wrapper<KBRelation>> relationRefs;
    std::vector<KBRule> rules;

    // add empty relations and relation ids to current KBModule
    for (auto &relDef : module.getRelations()) {
        unsigned relId = addRelation(modId, relDef);
        relationIds.push_back(relId);
        relationRefs.push_back({relations_[relId]});
    }

    // add rules to current KBModule
    for (auto &rule : module.getRules()) {
        rules.push_back(convertRule(modId, rule));
    }

    // add facts to relations
    for (auto &fact : module.getFacts()) {
        addFact(modId, fact);
    }

    for(auto& relationRef : relationRefs){
        relationRef.get().finalize();
    }

    modules_.emplace_back(module.getName(), modId, std::move(rules),
                          std::move(relationIds), std::move(relationRefs));
}


void KnowledgeBaseImpl::validateModule(const Module &module) const {
    if (moduleNamePool_.exists(module.getName())) {
        throw InvalidInputException("Module '", module.getName(),
                                    "' is already defined.");
    }
    std::map<std::string, std::reference_wrapper<const RelationDef>>
        relationDefMap;

    for (auto &relDef : module.getRelations()) {
        auto ret = relationDefMap.insert({relDef.getPredicateSymbol(), relDef});
        if (!ret.second) {
            throw InvalidInputException(
                "Relation '", relDef.getPredicateSymbol(),
                "' defined more than once in the module '", module.getName(),
                "'.");
        }
    }

    try {
        for (auto &rule : module.getRules()) {
            checkRule(relationDefMap, rule);
        }

        for (auto &fact : module.getFacts()) {
            checkFact(relationDefMap, fact);
        }
    } catch (InvalidInputException &e) {
        throw InvalidInputException("Error in module '", module.getName(),
                                    "': ", e.what());
    }
}

Module KnowledgeBase::retrieveModule(const std::string &moduleName) const {
    return impl_->retrieveModule(moduleName);
}

Module KnowledgeBaseImpl::retrieveModule(const std::string &moduleName) const {

    unsigned modId = queryModuleId(moduleName);
    const KBModule &kbm = modules_[modId];

    std::vector<GroundLiteral> facts;
    std::vector<Rule> rules;
    std::vector<RelationDef> relations;

    for (auto relId : kbm.relationIds) {
        auto &kbr = relations_[relId];
        const std::string relName = relSymPool_.get(relId).second;

        relations.push_back({relName, kbr.colDomains});

        for (auto &kbFact : relations_.at(relId).getFacts()) {
            // TODO remove toKBGroundLiteral
            facts.push_back(
                retrieveGroundLiteral(kbFact, relName, kbr.colDomains));
        }
    }

    const KBModule &kbModule = modules_[modId];
    for (auto &kbRule : kbModule.rules) {
        rules.push_back(convertKBRule(kbRule, kbModule));
    }

    return {moduleName, std::move(relations), std::move(rules),
            std::move(facts)};
}

std::vector<GroundLiteral>
    KnowledgeBase::retrieveRelation(const std::string &moduleName,
                                    const std::string &relationName) const {
    return impl_->retrieveRelation(moduleName, relationName);
}

std::vector<GroundLiteral>
    KnowledgeBaseImpl::retrieveRelation(const std::string &moduleName,
                                        const std::string &relationName) const {
    unsigned modId = queryModuleId(moduleName);
    return retrieveRelation(queryRelationId(modId, relationName));
}

std::vector<std::string> KnowledgeBase::listModules() const {
    std::vector<std::string> modules;
    for (auto &mod : impl_->modules_) {
        modules.push_back(mod.name);
    }
    return modules;
}

std::vector<RelationDef>
    KnowledgeBase::listRelations(const std::string &module) const {
    std::vector<RelationDef> relations;
    unsigned modId = impl_->queryModuleId(module);
    const KBModule &kbm = impl_->modules_[modId];
    for (auto relId : kbm.relationIds) {
        auto &kbr = impl_->relations_[relId];
        const std::string relName = impl_->relSymPool_.get(relId).second;

        relations.push_back({relName, kbr.colDomains});
    }

    return relations;
}


RelationDef KnowledgeBase::getRelationDef(const std::string &module,
                                          const std::string &rel) const {

    return impl_->getRelationDef(module, rel);
}
RelationDef KnowledgeBaseImpl::getRelationDef(const std::string &module,
                                              const std::string &rel) const {
    unsigned modId = queryModuleId(module);
    unsigned relId = queryRelationId(modId, rel);
    return {rel, relations_[relId].colDomains};
}

std::vector<GroundLiteral>
    KnowledgeBaseImpl::retrieveRelation(unsigned relId) const {
    const std::vector<TermType> colDomains = relations_.at(relId).colDomains;
    std::vector<GroundLiteral> result;
    auto &facts = relations_.at(relId).getFacts();
    result.reserve(facts.size());

    const std::string relName = relSymPool_.get(relId).second;
    for (auto &kbFact : facts) {
        // TODO remove toKBGroundLiteral
        result.push_back(retrieveGroundLiteral(kbFact, relName, colDomains));
    }
    return result;
}

void KnowledgeBase::addResultToKB(std::vector<KBTuple> newFacts,
                                  unsigned relId) {
    auto &relation = impl_->relations_[relId];

    for (auto &&tuple : newFacts) {
        relation.addTuple(std::move(tuple));
    }
}

void KnowledgeBaseImpl::addFact(unsigned modId, const GroundLiteral &fact) {
    KBGroundLiteral literal = convertGroundLiteral(modId, fact);
    relations_[literal.getRelId()].addEdbFact(literal);
}

unsigned KnowledgeBaseImpl::addRelation(unsigned modId,
                                        const RelationDef &def) {
    auto relId = relSymPool_.insert({modId, def.getPredicateSymbol()});
    if (!relId.second) {
        throw InvalidInputException("Relation is already defined (",
                                    def.getPredicateSymbol(), ").");
    }
    relations_.emplace_back(relId.first, def.getTypes());
    return relId.first;
}

KBTerm KnowledgeBaseImpl::convertTerm(const Term &term) {
    if (term.getType() == TermType::Var) {
        return KBTerm(termMapper_.internTerm(term), true);
    }
    return KBTerm(termMapper_.internTerm(term), false);
}

KBGroundTerm KnowledgeBaseImpl::convertGroundTerm(const Term &term) {
    assert(term.getType() != TermType::Var);
    return KBGroundTerm(termMapper_.internTerm(term));
}

KBHeadLiteral KnowledgeBaseImpl::convertLiteral(unsigned modId,
                                                const Literal &literal) {
    unsigned relId = queryRelationId(modId, literal.getPredicateSymbol());
    std::vector<KBTerm> kbTerms(literal.getTerms().size());

    Transform(literal.getTerms(), kbTerms.begin(),
              [this](const Term &t) { return this->convertTerm(t); });

    return KBHeadLiteral{literal.isTrue(), relId, std::move(kbTerms)};
}


TermType KnowledgeBaseImpl::queryTermType(unsigned relationId,
                                          unsigned idx) const {
    return relations_[relationId].colDomains[idx];
}

std::map<std::string, TermType>
    KnowledgeBaseImpl::makeTypeMap(const RuleDisjunct &disjunct,
                                   unsigned modId) const {

    std::map<std::string, TermType> typeMap;
    for (auto &literal : disjunct.getBody()) {
        if (literal.isFunction()) {
            continue;
        }
        unsigned idx = 0;
        unsigned literalModId = modId;
        if (literal.isExternal()) {
            literalModId = queryModuleId(*literal.getExternalModule());
        }
        for (auto &term : literal.getTerms()) {
            if (!IsVar(term)) {
                idx++;
                continue;
            }
            typeMap.insert(
                {term.get<std::string>(),
                 queryTermType(queryRelationId(literalModId,
                                               literal.getPredicateSymbol()),
                               idx)});
            idx++;
        }
    }
    return typeMap;
}

KBRule KnowledgeBaseImpl::convertRule(unsigned modId, const Rule &rule) {
    std::vector<KBRuleDisjunct> disjuncts;
    for (auto &disjunct : rule.getDisjuncts()) {
        std::map<std::string, TermType> typeMap = makeTypeMap(disjunct, modId);

        std::vector<KBRuleLiteral> disjunctLiterals;
        std::vector<KBRuleFunction> disjunctFunctions;
        for (auto &literal : disjunct.getBody()) {
            if (!literal.isFunction()) {
                disjunctLiterals.emplace_back(
                    convertRuleLiteral(modId, literal));
            } else {
                disjunctFunctions.emplace_back(
                    convertRuleFunction(literal, typeMap));
            }
        }
        disjuncts.emplace_back(std::move(disjunctLiterals),
                               std::move(disjunctFunctions));
    }

    return {convertLiteral(modId, rule.getHead()), std::move(disjuncts)};
}

KBQuery KnowledgeBase::convertQuery(const Query &query) const {
    return impl_->convertQuery(query);
}

KBQuery KnowledgeBaseImpl::convertQuery(const Query &query) const {
    unsigned modId = queryModuleId(query.moduleSymbol);
    unsigned relId = queryRelationId(modId, query.predicateSymbol);

    try {
        checkDomainsTerms(relations_[relId].colDomains, query.terms);
    } catch (InvalidInputException &e) {
        throw InvalidInputException(e.what(), " In query: ", query);
    }

    std::vector<KBTerm> kbTerms(query.terms.size());
    std::map<Term, unsigned> varMap;

    unsigned curVarId = 0;
    Transform(query.terms, kbTerms.begin(), [&](const Term &t) {
        if (t.getType() == TermType::Var) {
            auto it = varMap.find(t);
            if (it == varMap.end()) {
                varMap[t] = curVarId++;
            }
            return KBTerm(varMap[t], true);
        } else {
            try {
                unsigned id = termMapper_.queryTermId(t);
                return KBTerm(id, false);
            } catch (std::out_of_range) {
                throw InvalidInputException("Term '", t, "' not found");
            }
        }
    });

    return KBQuery(query.predicateSymbol, relId, std::move(kbTerms));
}

KBGroundLiteral
    KnowledgeBaseImpl::convertGroundLiteral(unsigned modId,
                                            const GroundLiteral &literal) {
    unsigned relId = queryRelationId(modId, literal.getPredicateSymbol());
    Array<KBGroundTerm> kbTerms(literal.getTerms().size());

    Transform(literal.getTerms(), kbTerms.begin(),
              [this](const Term &t) { return this->convertGroundTerm(t); });

    TruthValue logicVal = TruthValue::True;
    if (IsFalse(literal)) {
        logicVal = TruthValue::False;
    }

    return KBGroundLiteral(relId, std::move(kbTerms), logicVal);
}

KBRuleFunction KnowledgeBaseImpl::convertRuleFunction(
    const RuleLiteral &literal,
    const std::map<std::string, TermType> &typeMap) {

    assert(literal.isFunction() == true);

    std::vector<KBTerm> kbTerms;
    kbTerms.reserve(literal.getTerms().size());
    std::vector<TermType> termTypes(literal.getTerms().size());

    for (auto const &term : literal.getTerms()) {
        kbTerms.push_back(convertTerm(term));
        if (IsVar(term)) {
            assert(Contains(typeMap, term.get<std::string>()));
            termTypes.push_back(typeMap.find(term.get<std::string>())->second);
        } else {
            termTypes.push_back(term.getType());
        }
    }

    return {literal.isTrue(), literal.getPredicateFunction(),
            std::move(kbTerms), std::move(termTypes)};
}

KBRuleLiteral
    KnowledgeBaseImpl::convertRuleLiteral(unsigned modId,
                                          const RuleLiteral &literal) {
    unsigned relId;

    if (literal.isExternal()) {
        modId = queryModuleId(*literal.getExternalModule());
    }

    assert(literal.isFunction() == false);

    relId = queryRelationId(modId, literal.getPredicateSymbol());

    std::vector<KBTerm> kbTerms(literal.getTerms().size());

    Transform(literal.getTerms(), kbTerms.begin(),
              [this](const Term &t) { return this->convertTerm(t); });

    const TruthValueSet logicSet = literal.getLogicConstraint();
    TruthValueSet rlSet;

    if (IsFalse(literal) && logicSet.isEmpty()) {
        rlSet = TruthValueSet({TruthValue::False, TruthValue::Incons});
    } else if (IsFalse(literal)) {
        rlSet = logicSet.getNegation();
    } else if (logicSet.isEmpty()) {
        rlSet = TruthValueSet({TruthValue::True, TruthValue::Incons});
    } else {
        rlSet = logicSet;
    }

    return {relId, std::move(kbTerms), rlSet};
}

GroundLiteral
    KnowledgeBaseImpl::retrieveGroundLiteral(unsigned relId,
                                             const KBTuple &tuple) const {
    const std::vector<TermType> colDomains = relations_.at(relId).colDomains;
    const std::string relName = relSymPool_.get(relId).second;
    return retrieveGroundLiteral(tuple, relName, colDomains);
}

GroundLiteral KnowledgeBaseImpl::retrieveGroundLiteral(
    const KBTuple &tuple, const std::string &relName,
    const std::vector<TermType> &types) const {
    std::vector<Term> terms;
    auto &kbTerms = tuple.getTerms();
    terms.reserve(kbTerms.size());
    for (size_t i = 0; i < kbTerms.size(); i++) {
        terms.push_back(termMapper_.queryTerm(kbTerms[i].get(), types[i]));
    }

    bool t = true;
    if (tuple.getValue() == TruthValue::False) {
        t = false;
    }

    return GroundLiteral(t, relName, std::move(terms));
}

ResultFact
    KnowledgeBase::makeResultFact(Array<KBGroundTerm> tuple, TruthValue value,
                                  const std::vector<TermType> &types) const {
    return impl_->makeResultFact(tuple, value, types);
}

ResultFact KnowledgeBaseImpl::makeResultFact(
    const Array<KBGroundTerm> &tuple, TruthValue value,
    const std::vector<TermType> &types) const {

    Array<Term> terms(tuple.size());
    for (size_t i = 0; i < tuple.size(); i++) {
        terms[i] = termMapper_.queryTerm(tuple[i].get(), types[i]);
    }
    return {std::move(terms), value};
}

Rule KnowledgeBaseImpl::convertKBRule(const KBRule &rule,
                                      const KBModule &module) const {

    std::vector<RuleDisjunct> ruleDisjuncts;
    for (auto &disjunct : rule.getDisjuncts()) {
        std::vector<RuleLiteral> disjunctBody;
        disjunctBody.reserve(disjunct.getBody().size());
        Transform(disjunct.getBody(), std::back_inserter(disjunctBody),
                  [&](const KBRuleLiteral &kbRuleLiteral) {
            return convertKBRuleLiteral(kbRuleLiteral, module);
        });
        ruleDisjuncts.emplace_back(std::move(disjunctBody));
    }

    return {convertKBHeadLiteral(rule.getHead()), std::move(ruleDisjuncts)};
}

Literal KnowledgeBaseImpl::convertKBHeadLiteral(
    const KBHeadLiteral &literal) const {

    const std::string relName = relSymPool_.get(literal.getRelId()).second;
    const std::vector<TermType> &types =
        relations_[literal.getRelId()].colDomains;

    std::vector<Term> terms;
    terms.reserve(literal.getTerms().size());
    for (size_t i = 0; i < literal.getTerms().size(); i++) {
        terms.push_back(
            termMapper_.queryTerm(literal.getTerms()[i].get(), types[i]));
    }

    return Literal(!literal.isNegated(), relName, std::move(terms));
}

RuleLiteral
    KnowledgeBaseImpl::convertKBRuleLiteral(const KBRuleLiteral &literal,
                                            const KBModule &module) const {

    const std::string relName = relSymPool_.get(literal.getRelId()).second;
    const std::vector<TermType> &types =
        relations_[literal.getRelId()].colDomains;

    std::vector<Term> terms;
    for (size_t i = 0; i < literal.getTerms().size(); i++) {
        terms.push_back(
            termMapper_.queryTerm(literal.getTerms()[i].get(), types[i]));
    }

    if (Find(module.relationIds, literal.getRelId()) !=
        module.relationIds.end()) {
        return RuleLiteral(!literal.getLogicSet().isFalse(), relName,
                           std::move(terms));
    }

    std::vector<TruthValue> lValueSet = literal.getLogicSet().getSet();

    if (lValueSet.size() == 1 || lValueSet[0] == TruthValue::False ||
        lValueSet[0] == TruthValue::True) {
        return RuleLiteral(!literal.getLogicSet().isFalse(), module.name,
                           relName, std::move(terms));
    }

    return RuleLiteral(module.name, relName, std::move(terms), lValueSet);
}


unsigned KnowledgeBaseImpl::queryModuleId(const std::string &module) const {
    unsigned modId;
    try {
        modId = moduleNamePool_.queryId(module);
    } catch (std::out_of_range &e) {
        throw InvalidInputException("No such module: ", module);
    }
    return modId;
}

unsigned KnowledgeBaseImpl::queryRelationId(unsigned modId,
                                            const std::string &rel) const {
    unsigned relId;
    try {
        relId = relSymPool_.queryId({modId, rel});
    } catch (std::out_of_range &e) {
        throw InvalidInputException("No such relation '", rel, "' in module '",
                                    moduleNamePool_.get(modId), "'");
    }
    return relId;
}
} // fovris
