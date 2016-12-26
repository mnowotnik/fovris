#ifndef MODULE_H_JPS17C05
#define MODULE_H_JPS17C05
#include "GroundLiteral.h"
#include "RelationDef.h"
#include "Rule.h"

#include <iosfwd>
#include <string>
#include <vector>

namespace fovris {

class Module {
    std::string name;
    std::vector<RelationDef> relations;
    std::vector<Rule> rules;
    std::vector<GroundLiteral> facts;

  public:
    Module(std::string name, std::vector<RelationDef> relations = {},
           std::vector<Rule> rules = {}, std::vector<GroundLiteral> facts = {})
        : name(std::move(name))
        , relations(std::move(relations))
        , rules(std::move(rules))
        , facts(std::move(facts)) {}

    const decltype(name) &getName() const { return name; }
    const decltype(relations) &getRelations() const { return relations; }
    const decltype(rules) &getRules() const { return rules; }
    const decltype(facts) &getFacts() const { return facts; }

    friend std::ostream &operator<<(std::ostream &os, const Module &m);
};
} // fovris
#endif /* end of include guard: MODULE_H_JPS17C05 */
