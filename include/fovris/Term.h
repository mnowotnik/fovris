#ifndef TERM_H_QXERYYQR
#define TERM_H_QXERYYQR
#include <cstdint>
#include <regex>

#include "DateTime.h"
#include "Exceptions.h"
#include "TruthValue.h"
#include "make_unique.hpp"

namespace fovris {
enum class TermType : uint8_t {
    Integer = 0,
    Real,
    Logic,
    String,
    Id,
    Date,
    DateTime,
    Var
};

std::ostream &operator<<(std::ostream &os, TermType t);

class Term {
  protected:
    union Val_ {
        double d_;
        int i_;
        TruthValue t_;
        std::string *s_;
        DateTime dateTime_;
        Date date_;

        Val_() {}
        Val_(double d) : d_(d) {}
        Val_(int i) : i_(i) {}
        Val_(TruthValue t) : t_(t) {}
        Val_(DateTime dt) : dateTime_(dt) {}
        Val_(Date d) : date_(d) {}
        Val_(std::unique_ptr<std::string> s) : s_(s.release()) {}
    } val_;

    TermType type;
    void swap(Term &other);

  public:
    Term();
    Term(int i);
    Term(TruthValue t);
    Term(double d);
    Term(DateTime dt);
    Term(Date d);

    Term(const std::string &s, TermType type);
    Term(const char *s, TermType type);
    Term(std::unique_ptr<std::string> s, TermType type);

    Term(std::unique_ptr<std::string> s);
    Term(const char *s);
    Term(const std::string &s);

    Term(Term &&that) noexcept;

    Term(const Term &that) throw(std::bad_alloc);

    Term &operator=(Term that);

    ~Term();

    bool operator==(const Term &o) const;
    bool operator!=(const Term &o) const;
    bool operator<(const Term &o) const;
    bool operator>(const Term &o) const;

    TermType getType() const { return type; }

    template <typename T> T get() const;

    std::ostream &print(std::ostream &os, bool typeInfo) const;

    friend std::ostream &operator<<(std::ostream &os, const Term &t);
};

class Var : public Term {
  public:
    Var(std::unique_ptr<std::string> s);
    Var(const char *s);
    Var(const std::string &s);
};

template <> TruthValue Term::get() const;
template <> int Term::get() const;
template <> double Term::get() const;
template <> const std::string &Term::get() const;
template <> std::string Term::get() const;
template <> DateTime Term::get() const;
template <> Date Term::get() const;

bool CanBe(const Term &term, TermType type);
bool IsVar(const Term &term);


} // fovris
#endif /* end of include guard: TERM_H_QXERYYQR */
