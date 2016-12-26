#ifndef KBTERM_H_5NBPEFUC
#define KBTERM_H_5NBPEFUC
namespace fovris {
class KBTerm {
    unsigned val_;
    bool isVar_;

  public:
    KBTerm() : val_(0xDEADBEEF), isVar_(false) {}

    explicit KBTerm(unsigned val, bool isVar = false)
        : val_(val), isVar_(isVar) {}

    unsigned get() const { return val_; }

    bool isVariable() const { return isVar_; }
    bool isNull() const;

    bool operator<(const KBTerm &o) const { return val_ < o.val_; }

    bool operator>(const KBTerm &o) const { return val_ > o.val_; }

    bool operator==(const KBTerm &o) const { return val_ == o.val_; }

    bool operator!=(const KBTerm &o) const { return val_ != o.val_; }
};

class KBVarTerm : public KBTerm {
  public:
    using KBTerm::KBTerm;
    explicit KBVarTerm(unsigned val) : KBTerm(val, true) {}
};

class KBNullTerm: public KBTerm{
    public:
        KBNullTerm();
};

} // fovris
#endif /* end of include guard: KBTERM_H_5NBPEFUC */
