#ifndef JOIN_H_KWRZ249H
#define JOIN_H_KWRZ249H
#include "KB/KBRuleLiteral.h"

namespace fovris {

class JoinInfo {
  public:
    const KBRuleLiteral &rel;
    const std::set<unsigned> includedIndices;
    JoinInfo(const KBRuleLiteral &rel, std::set<unsigned> includedIndices);

    JoinInfo(const KBRuleLiteral &rel);
};

class Join {

    // pairs of indices indicating terms to be compared for equality in
    // joined relations
    std::vector<std::pair<unsigned, unsigned>> equalIndices;
    std::vector<unsigned> leftIndices;
    std::vector<unsigned> rightIndices;

  public:
    Join(const KBRuleLiteral &relA, const KBRuleLiteral &relB) {
        makeEqualIndices(JoinInfo{relA}, JoinInfo{relB});
    }

    Join(const JoinInfo &infoA, const KBRuleLiteral &relB) {
        makeEqualIndices(infoA, JoinInfo{relB});
    }

    Join(const KBRuleLiteral &relA, const JoinInfo &infoB) {
        makeEqualIndices(JoinInfo{relA}, infoB);
    }

    template <typename TermArray>
    bool joinable(const TermArray &tupA, const TermArray &tupB) const {
        for (auto &condPair : equalIndices) {
            if (tupA[condPair.first] != tupB[condPair.second] &&
                !tupA[condPair.first].isNull() &&
                !tupB[condPair.second].isNull()) {
                return false;
            }
        }
        return true;
    }

    template <typename TermArray>
    int compare(const TermArray &tupA, const TermArray &tupB) {
        auto it = equalIndices.begin();
        for (; it != equalIndices.end(); it++) {
            if (tupA[it->first] != tupB[it->second] &&
                !tupA[it->first].isNull() && !tupB[it->second].isNull()) {
                break;
            }
        }

        if (it == equalIndices.end()) {
            return 0;
        }

        if (tupA[it->first] < tupB[it->second]) {
            return -1;
        }

        return 1;
    }

    template <typename TermArray>
    int compareWithoutNull(const TermArray &tupA, const TermArray &tupB) {
        auto it = equalIndices.begin();
        for (; it != equalIndices.end(); it++) {
            if (tupA[it->first] != tupB[it->second]) {
                break;
            }
        }

        if (it == equalIndices.end()) {
            return 0;
        }

        if (tupA[it->first] < tupB[it->second]) {
            return -1;
        }

        return 1;
    }


    template <typename TermArray>
    bool joinableWithoutNull(const TermArray &tupA,
                             const TermArray &tupB) const {
        for (auto &condPair : equalIndices) {
            if (tupA[condPair.first] != tupB[condPair.second]) {
                return false;
            }
        }
        return true;
    }

    const decltype(equalIndices) &getEqualIndices() { return equalIndices; }
    const decltype(leftIndices) &getLeftRelIndices() { return leftIndices; }
    const decltype(rightIndices) &getRightRelIndices() { return rightIndices; }

  private:
    void makeEqualIndices(const JoinInfo &infoA, const JoinInfo &infoB);
};


} // fovris
#endif /* end of include guard: JOIN_H_KWRZ249H */
