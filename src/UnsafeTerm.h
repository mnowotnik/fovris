#include "Term.h"

namespace fovris {

class UnsafeTerm : public Term {
  public:
    using Term::Term;
    UnsafeTerm(const std::string &s, TermType type)
        : Term(std::make_unique<std::string>(s), type) {}
    UnsafeTerm(const char *s, TermType type)
        : Term(std::make_unique<std::string>(s), type) {}
    UnsafeTerm(std::unique_ptr<std::string> s, TermType type)
        : Term(std::move(s), type) {}
};
} // fovris
