#ifndef PROJECTION_H_AJ0GDVPQ
#define PROJECTION_H_AJ0GDVPQ

#include "Eval/Utils.h"
#include "KB/KBRuleLiteral.h"
namespace fovris {

template<class T> using remove_const_ref =
  std::remove_reference<typename std::remove_cv<T>::type>;

class Projection {
    enum IdType { Var, Constant };
    struct ProjectionId {
        unsigned id;
        IdType type;
    };
    std::vector<ProjectionId> projectionIds;

  public:
    template <typename Container>
    Projection(const KBRuleLiteral &source, const Container &destTerms)
        : projectionIds(makeProjectionIds(source, destTerms)) {}

    template <typename T, template <typename, typename...> class V,
              typename... Args>
    V<T, Args...> project(const V<T, Args...> &terms) const {
        V<T,Args...> projection(projectionIds.size());
        size_t i = 0;
        for (auto id : projectionIds) {
            if (id.type == IdType::Constant) {
                projection[i] = T(id.id);
            } else {
                projection[i] = terms[id.id];
            }
            i++;
        }
        return projection;
    }

  private:
    template <typename Container>
    decltype(projectionIds) makeProjectionIds(const KBRuleLiteral &literal,
                                              const Container &destTerms) {

        auto projectionIds = decltype(this->projectionIds){};
        auto const &varIndices = literal.getVarIndices();
        for (auto &term : destTerms) {
            if (term.isVariable()) {
                auto vIt = varIndices.find(term);
                if (vIt != varIndices.end()) {
                    projectionIds.push_back({vIt->second.front(), IdType::Var});
                }
            } else {
                projectionIds.push_back({term.get(), IdType::Constant});
            }
        }
        return projectionIds;
    }
};


} // fovris
#endif /* end of include guard: PROJECTION_H_AJ0GDVPQ */
