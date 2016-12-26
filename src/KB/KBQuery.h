#include "KB/KBLiteral.h"
namespace fovris {

#ifndef KBQUERY_H_0YEIBI2O
#define KBQUERY_H_0YEIBI2O

class KBQuery : public KBLiteral {

    std::string relationName;

  public:
    KBQuery(std::string relName, unsigned relId, std::vector<KBTerm> terms);

    const decltype(relationName) &getRelationName() const {
        return relationName;
    }
};
} // fovris
#endif /* end of include guard: KBQUERY_H_0YEIBI2O */
