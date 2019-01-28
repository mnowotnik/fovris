#include "TermMapper.h"
#include "UnsafeTerm.h"

namespace fovris {

unsigned TermMapper::internTerm(const Term &term) {
    switch (term.getType()) {
        case TermType::Var:
        case TermType::String:
        case TermType::Id:
            return strTermPool_.insert(term.get<std::string>()).first;
        case TermType::Date:
            return dateTermPool_.insert(term.get<Date>()).first;
        case TermType::DateTime:
            return dateTimeTermPool_.insert(term.get<DateTime>()).first;
        case TermType::Real:
            return dblTermPool_.insert(term.get<double>()).first;
        case TermType::Integer:
            return static_cast<unsigned>(term.get<int>());
        case TermType::Logic:
            return static_cast<unsigned>(term.get<TruthValue>());
        default:
            return 0xDEADBEEF;
    }
}

Term TermMapper::queryTerm(unsigned id, TermType type) const {
    switch (type) {
        case TermType::Var:
        case TermType::Id:
        case TermType::String:
            return Term{strTermPool_.get(id), type};
        case TermType::Date:
            return Term{dateTermPool_.get(id)};
        case TermType::DateTime:
            return Term{dateTimeTermPool_.get(id)};
        case TermType::Real:
            return Term{dblTermPool_.get(id)};
        case TermType::Integer:
            return Term{static_cast<int>(id)};
        case TermType::Logic:
            return Term{static_cast<TruthValue>(id)};
        default:
            return Term{};
    }
}

unsigned TermMapper::queryTermId(const Term &term) const {
    switch (term.getType()) {
        case TermType::Var:
        case TermType::String:
        case TermType::Id:
            return strTermPool_.queryId(term.get<std::string>());
        case TermType::Date:
            return dateTermPool_.queryId(term.get<Date>());
        case TermType::DateTime:
            return dateTimeTermPool_.queryId(term.get<DateTime>());
        case TermType::Real:
            return dblTermPool_.queryId(term.get<double>());
        case TermType::Integer:
            return static_cast<unsigned>(term.get<int>());
        case TermType::Logic:
            return static_cast<unsigned>(term.get<TruthValue>());
        default:
            return 0xDEADBEEF;
    }
}
} // fovris
