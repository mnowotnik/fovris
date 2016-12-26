#include "Program.h"
#include <memory>
#include <unordered_set>
#include <functional>

#include "KnowledgeBase.h"
#include "Utils.h"
#include "Evaluation.h"
#include "NaiveEvaluation.h"
#include "SemiNaiveEvaluation.h"
namespace fovris {

class Program::ProgramImpl {
    KnowledgeBase kb_;
    std::unique_ptr<Evaluation> evaluation;

  public:
    ProgramImpl(Algorithm algorithm);

    /* ProgramImpl(ProgramImpl &&move) = default; */
    /* ProgramImpl(const ProgramImpl &program) */
    /*     : kb_(program.kb_) */
    /*     ,
     * evaluation(std::unique_ptr<Evaluation>(program.evaluation->clone())) { */
    /* } */

    void addModule(const Module &module);
    void evaluate();
    QueryResult query(const Query &query) const;
    void clear();

    std::vector<std::string> listModules() const;
    std::vector<RelationDef> listRelations(const std::string &module) const;
};

Program::ProgramImpl::ProgramImpl(Algorithm algorithm) {
    switch (algorithm) {
        case Algorithm::Naive:
            evaluation = std::make_unique<NaiveEvaluation>();
            break;
        case Algorithm::Seminaive:
            evaluation = std::make_unique<SemiNaiveEvaluation>();
            break;
    }
}

void Program::ProgramImpl::addModule(const Module &module) {
    kb_.addModule(module);
}

std::vector<std::string> Program::ProgramImpl::listModules() const {
    return kb_.listModules();
}
std::vector<RelationDef>
    Program::ProgramImpl::listRelations(const std::string &module) const {
    return kb_.listRelations(module);
}

void Program::ProgramImpl::clear() { kb_ = KnowledgeBase(); }

void Program::ProgramImpl::evaluate() { evaluation->evaluateKB(kb_); }

QueryResult Program::ProgramImpl::query(const Query &query) const {
    KBQuery kbQuery = kb_.convertQuery(query);
    return {query, evaluation->queryKB(kbQuery, kb_)};
}

Program::Program(Algorithm algorithm)
    : impl(std::make_unique<ProgramImpl>(algorithm)) {}

Program::Program(Program &&program) : impl(std::move(program.impl)) {}

Program::~Program() {}

QueryResult Program::query(const std::string &module,
                           const std::string &predicate,
                           const std::vector<Term> &terms) const {
    return query(Query{module, predicate, terms});
}

QueryResult Program::query(const std::string &rawQuery) const {
    return query(Query{rawQuery});
}

QueryResult Program::query(const Query &query) const {
    return impl->query(query);
}

void Program::addModule(const Module &module) { impl->addModule(module); }
void Program::addModules(const std::vector<Module> &modules) {
    for (auto &module : modules) {
        addModule(module);
    }
}


void Program::evaluate() { impl->evaluate(); }
void Program::clear() { impl->clear(); }

std::vector<std::string> Program::listModules() const {
    return impl->listModules();
}
std::vector<RelationDef>
    Program::listRelations(const std::string &module) const {
    return impl->listRelations(module);
}
} // fovris
