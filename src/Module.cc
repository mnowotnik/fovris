#include "Module.h"
#include "GroundLiteral.h"
#include "RelationDef.h"
#include "Rule.h"

#include <ostream>
namespace fovris {


namespace {

template <typename T>
std::ostream &ostreamPrintVec(std::ostream &os, const std::vector<T> &vec,
                              int indent = 4) {
    const std::string indentStr(indent, ' ');
    for (auto &el : vec) {
        os << indentStr << el << '.' << std::endl;
    }
    return os;
}

} // namespace

std::ostream &operator<<(std::ostream &os, const std::vector<GroundLiteral> &vec) {
    return ostreamPrintVec(os, vec);
}
std::ostream &operator<<(std::ostream &os, const std::vector<Rule> &vec) {
    return ostreamPrintVec(os, vec);
}
std::ostream &operator<<(std::ostream &os,
                         const std::vector<RelationDef> &vec) {
    return ostreamPrintVec(os, vec);
}

std::ostream &operator<<(std::ostream &os, const Module &m) {
    os << "module " << m.name << ':' << std::endl;
    os << "  relations:" << std::endl;
    os << m.relations;
    os << "  rules:" << std::endl;
    os << m.rules;
    os << "  facts:" << std::endl;
    os << m.facts;
    return os;
}
} // fovris
