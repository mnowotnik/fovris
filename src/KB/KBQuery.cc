#include "KB/KBQuery.h"
namespace fovris {

KBQuery::KBQuery(std::string relName, unsigned relId, std::vector<KBTerm> terms)
    : KBLiteral(relId, std::move(terms)), relationName(std::move(relName)) {}

} // fovris
