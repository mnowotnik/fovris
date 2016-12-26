#ifndef DESERIALIZER_H_SPXUFOI5
#define DESERIALIZER_H_SPXUFOI5
#include "Module.h"
#include "Query.h"
namespace fovris {
class Deserializer {
  public:
    virtual std::vector<Query> getQueries() = 0;
    virtual std::vector<Module>&& getModules() = 0;
    virtual void parse(const std::string &programStr) = 0;
};
} // fovris

#endif /* end of include guard: DESERIALIZER_H_SPXUFOI5 */
