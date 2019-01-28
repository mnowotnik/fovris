#include "Term.h"
#include <regex>
namespace fovris {

namespace {
const std::regex VarRegex("[A-Z]\\w*");
const std::regex IdRegex("[a-z_]+[\\w_]*");
inline bool isStrType(const Term &term) {
    switch (term.getType()) {
        case TermType::String:
        case TermType::Var:
        case TermType::Id:
            return true;
        case TermType::Real:
        case TermType::Logic:
        case TermType::Date:
        case TermType::DateTime:
        case TermType::Integer:
            return false;
        default:
            return false;
    }
    return false;
}
std::regex quoteRegex("\"");
std::string escapeQuotes(const std::string &str) {
    return std::regex_replace(str, quoteRegex, "\\\"");
}

bool hasQuotes(const std::string &str) {
    return str.find('"') != std::string::npos;
}


} // namespace


void Term::swap(Term &other) {
    using std::swap;
    swap(type, other.type);
    swap(val_, other.val_);
}

// public ctors
Term::Term() : val_(0), type(TermType::Integer) {}
Term::Term(int i) : val_(i), type(TermType::Integer) {}
Term::Term(TruthValue t) : val_(t), type(TermType::Logic) {}
Term::Term(double d) : val_(d), type(TermType::Real) {}
Term::Term(DateTime dt) : val_(dt), type(TermType::DateTime) {}
Term::Term(Date dt) : val_(dt), type(TermType::Date) {}

Term::Term(std::unique_ptr<std::string> s)
    : Term(std::move(s), TermType::String) {}
Term::Term(const char *s) : Term(std::string(s)) {}
Term::Term(const std::string &s) : Term(s, TermType::String) {}

Term::Term(const std::string &s, TermType type)
    : Term(std::make_unique<std::string>(s), type) {}
Term::Term(const char *s, TermType type)
    : Term(std::make_unique<std::string>(s), type) {}
Term::Term(std::unique_ptr<std::string> s, TermType type)
    : val_(std::move(s)), type(type) {
    if (type != TermType::String && type != TermType::Var &&
        type != TermType::Id) {
        throw InvalidInputException(
            "string can only have Var,Id or String types");
    }
}

Var::Var(std::unique_ptr<std::string> s) : Term(std::move(s), TermType::Var) {}
Var::Var(const char *s) : Term(s, TermType::Var) {}
Var::Var(const std::string &s) : Term(s, TermType::Var) {}


Term::Term(Term &&that) noexcept : val_(that.val_), type(that.type) {
    if (isStrType(*this)) {
        that.val_.s_ = nullptr;
    }
}

Term::Term(const Term &that) : type(that.type) {
    if (isStrType(*this)) {
        val_.s_ = new std::string(*that.val_.s_);
    } else {
        val_ = that.val_;
    }
}

Term::~Term() {
    if (isStrType(*this)) {
        delete val_.s_;
    }
}

Term &Term::operator=(Term that) {
    swap(that);
    return *this;
}

bool IsVar(const Term &term) { return term.getType() == TermType::Var; }

bool CanBe(const Term &term, TermType type) {
    if (term.getType() == type) {
        return true;
    }

    if (term.getType() != TermType::String) {
        return false;
    }

    switch (type) {
        case TermType::Id:
            return std::regex_match(term.get<std::string>(), IdRegex);
        case TermType::Var:
            return std::regex_match(term.get<std::string>(), VarRegex);
        default:
            return false;
    }
}
std::ostream &Term::print(std::ostream &os, bool typeInfo) const {
    switch (type) {
        case TermType::String:
            if (hasQuotes(*val_.s_)) {
                os << '"' << escapeQuotes(*val_.s_) << '"';
                break;
            }

            os << '"' << *val_.s_ << '"';
            break;
        case TermType::Var:
        case TermType::Id:
            os << *val_.s_;
            break;
        case TermType::Date:
            os << val_.date_;
            break;
        case TermType::DateTime:
            os << val_.dateTime_;
            break;
        case TermType::Integer:
            os << val_.i_;
            break;
        case TermType::Real:
            os << val_.d_;
            break;
        case TermType::Logic:
            os << val_.t_;
            break;
    }
    if (typeInfo) {
        os << ' ' << type;
    }
    return os;
}

bool Term::operator==(const Term &o) const {
    if (o.type != type) {
        // both are non-string types
        if (o.type < TermType::String || type < TermType::String) {
            return false;
        }

        // both are specific string types
        if (o.type > TermType::String && type > TermType::String) {
            return false;
        }
    }

    switch (type) {
        case TermType::String:
        case TermType::DateTime:
        case TermType::Date:
        case TermType::Var:
        case TermType::Id:
            return *val_.s_ == *o.val_.s_;
        case TermType::Integer:
            return val_.i_ == o.val_.i_;
        case TermType::Real:
            return val_.d_ == o.val_.d_;
        case TermType::Logic:
            return val_.t_ == o.val_.t_;
        default:
            return false;
    }
}

bool Term::operator!=(const Term &o) const { return !operator==(o); }

bool Term::operator<(const Term &o) const {
    if (o.type != type) {
        return type < o.type;
    }

    switch (type) {
        case TermType::String:
        case TermType::DateTime:
        case TermType::Date:
        case TermType::Var:
        case TermType::Id:
            return *val_.s_ < *o.val_.s_;
        case TermType::Integer:
            return val_.i_ < o.val_.i_;
        case TermType::Real:
            return val_.d_ < o.val_.d_;
        case TermType::Logic:
            return val_.t_ < o.val_.t_;
        default:
            return false;
    }
}

bool Term::operator>(const Term &o) const { return !operator<(o); }

template <> TruthValue Term::get() const {
    if (type != TermType::Logic) {
        throw IncorrectValueException("Wrong value type");
    }
    return val_.t_;
}

template <> int Term::get() const {
    if (type != TermType::Integer) {
        throw IncorrectValueException("Wrong value type");
    }
    return val_.i_;
}

template <> double Term::get() const {
    if (type != TermType::Real) {
        throw IncorrectValueException("Wrong value type");
    }
    return val_.d_;
}

template <> const std::string &Term::get() const {
    if (type != TermType::String && type != TermType::Var &&
        type != TermType::Id) {
        throw IncorrectValueException("Wrong value type");
    }
    return *val_.s_;
}

template <> std::string Term::get() const { return get<const std::string &>(); }


template <> DateTime Term::get() const {
    if (type != TermType::DateTime) {
        throw IncorrectValueException("Wrong value type");
    }
    return val_.dateTime_;
}

template <> Date Term::get() const {
    if (type != TermType::Date) {
        throw IncorrectValueException("Wrong value type");
    }
    return val_.date_;
}

std::ostream &operator<<(std::ostream &os, TermType t) {
    switch (t) {
        case TermType::Integer:
            os << "integer";
            break;
        case TermType::Real:
            os << "real";
            break;
        case TermType::String:
            os << "string";
            break;
        case TermType::Date:
            os << "date";
            break;
        case TermType::DateTime:
            os << "dateTime";
            break;
        case TermType::Var:
            os << "var";
            break;
        case TermType::Logic:
            os << "logic";
            break;
        case TermType::Id:
            os << "identifier";
            break;
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const Term &t) {
    return t.print(os, false);
}
} // fovris
