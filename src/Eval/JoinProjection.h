#ifndef JOINPROJECTION_H_KBQCXLOH
#define JOINPROJECTION_H_KBQCXLOH

#include "Eval/Utils.h"

#include "KB/KBRuleLiteral.h"
#include "KB/KBGroundTerm.h"

#include <cassert>
#include <type_traits>


#define TMPL_T_V                                                               \
    template <typename T, template <typename, typename...> class V,            \
              typename... Args>
#define TYPE_V V<T, Args...>


namespace fovris {


class JoinProjection {
    enum IdType { Constant, Left, Right, Both };
    struct ProjectionId {
        union {
            unsigned idOrConstant;
            std::pair<unsigned, unsigned> ids;
        };
        IdType idType;

        ProjectionId(unsigned idLeft, unsigned idRight)
            : ids{idLeft, idRight}, idType(IdType::Both) {}
        ProjectionId(unsigned idOrConstant, IdType idType)
            : idOrConstant(idOrConstant), idType(idType) {}
    };


  public:
    template <typename Container>
    JoinProjection(const KBRuleLiteral &left, const KBRuleLiteral &right,
                   const Container &destTerms)
        : projectionIds(makeProjectionIds(left, right, destTerms)) {}

    JoinProjection() {}

    TMPL_T_V
    TYPE_V project(const TYPE_V &termsL, const TYPE_V &termsR) const {
        TYPE_V projection(projectionIds.size());
        size_t i = 0;
        for (auto &colId : projectionIds) {
            switch (colId.idType) {
                case IdType::Constant:
                    projection[i] = T(colId.idOrConstant);
                    break;
                case IdType::Left:
                    assert(colId.idOrConstant < termsL.size());
                    projection[i] = termsL[colId.idOrConstant];
                    break;
                case IdType::Right:
                    assert(colId.idOrConstant < termsR.size());
                    projection[i] = termsR[colId.idOrConstant];
                    break;
                case IdType::Both:
                    // while joining only temp relation can have null values
                    // so no need to store and check more than 1 index from each relation
                    projection[i] = termsL[colId.ids.first];
                    if (projection[i].isNull()) {
                        projection[i] = termsR[colId.ids.second];
                    }
            }
            i++;
        }
        return projection;
    }

    // Project left part of join, subsituting missing values of the right part with Null values
    TMPL_T_V
    TYPE_V leftComplement(const TYPE_V &terms) const {
        TYPE_V projection(projectionIds.size());
        int i = 0;
        for (auto &colId : projectionIds) {
            switch (colId.idType) {
                case IdType::Constant:
                    projection[i] = T(colId.idOrConstant);
                    break;
                case IdType::Left:
                case IdType::Both:
                    assert(colId.idOrConstant < projection.size());
                    projection[i] = terms[colId.idOrConstant];
                    break;
                case IdType::Right:
                    projection[i] = KBGroundNullTerm();
                    break;
            }
            i++;
        }
        return projection;
    }

    // Project right part of join, subsituting missing values of the left part with Null values
    TMPL_T_V
    TYPE_V righComplement(const TYPE_V &terms) const {
        TYPE_V projection(projectionIds.size());
        int i = 0;
        for (auto &colId : projectionIds) {
            switch (colId.idType) {
                case IdType::Constant:
                    projection[i] = T(colId.idOrConstant);
                    break;
                case IdType::Right:
                case IdType::Both:
                    assert(colId.idOrConstant < projection.size());
                    projection[i] = terms[colId.idOrConstant];
                    break;
                case IdType::Left:
                    projection[i] = KBGroundNullTerm();
                    break;
            }
            i++;
        }
        return projection;
    }

  private:
    std::vector<ProjectionId> projectionIds;

    template <typename Container>
    decltype(projectionIds) makeProjectionIds(const KBRuleLiteral &left,
                                              const KBRuleLiteral &right,
                                              const Container &destTerms) {

        auto projectionIds = decltype(this->projectionIds){};
        auto const &fVarIndices = left.getVarIndices();
        auto const &sVarIndices = right.getVarIndices();
        for (auto &term : destTerms) {
            if (term.isVariable()) {
                auto fIt = fVarIndices.find(term);
                auto sIt = sVarIndices.find(term);
                if (fIt != fVarIndices.end() && sIt == sVarIndices.end()) {
                    projectionIds.emplace_back(fIt->second.front(),
                                               IdType::Left);
                    continue;
                } else if (sIt != sVarIndices.end() &&
                           fIt == fVarIndices.end()) {
                    projectionIds.emplace_back(sIt->second.front(),
                                               IdType::Right);
                } else if (sIt != sVarIndices.end() &&
                           fIt != fVarIndices.end()) {
                    projectionIds.emplace_back(fIt->second.front(),
                                               sIt->second.front());
                }
            } else {
                projectionIds.emplace_back(term.get(), IdType::Constant);
            }
        }
        return projectionIds;
    }
};


#undef TMPL_T_V
#undef TYPE_V

} // fovris
#endif /* end of include guard: JOINPROJECTION_H_KBQCXLOH */
