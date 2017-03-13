#ifndef PROGRAM_H_X1BYNOLP
#define PROGRAM_H_X1BYNOLP
#include <memory>

#include "QueryResult.h"
#include "Module.h"
namespace fovris {

enum Algorithm { Naive, Seminaive };

class Program {
    class ProgramImpl;
    std::unique_ptr<ProgramImpl> impl;

  public:
    Program(Algorithm algorithm);
    Program(Program &&program);
    Program(const Program &program) = delete;

    ~Program();

    void addModule(const Module &module);
    void addModules(const std::vector<Module> &modules);

    std::vector<std::string> listModules() const;
    std::vector<RelationDef> listRelations(const std::string &module) const;

    void evaluate();

    void clear();

    QueryResult query(const std::string &module, const std::string &predicate,
                      const std::vector<Term> &terms) const;

    QueryResult query(const Query &query) const;
    QueryResult query(const std::string &rawQuery) const;
};

} // fovris
#endif /* end of include guard: PROGRAM_H_X1BYNOLP */
