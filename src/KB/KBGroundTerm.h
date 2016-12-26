#ifndef KBGROUNDTERM_H_GEVFLUTE
#define KBGROUNDTERM_H_GEVFLUTE
namespace fovris {
class KBGroundTerm {
    unsigned val_;
  public:
    KBGroundTerm() {}

    explicit KBGroundTerm(unsigned val) : val_(val) {}

    unsigned get() const { return val_; }

    bool isNull() const;

    bool operator<(const KBGroundTerm &o) const { return val_ < o.val_; }

    bool operator>(const KBGroundTerm &o) const { return val_ > o.val_; }

    bool operator==(const KBGroundTerm &o) const { return val_ == o.val_; }


    bool operator!=(const KBGroundTerm &o) const { return val_ != o.val_; }
};


class KBGroundNullTerm: public KBGroundTerm{
    public:
        KBGroundNullTerm();
};
} // fovris
#endif /* end of include guard: KBGROUNDTERM_H_GEVFLUTE */
