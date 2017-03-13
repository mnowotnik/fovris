#ifndef QLDESERIALIZER_H_FWVEGBMK
#define QLDESERIALIZER_H_FWVEGBMK
#include "Deserializer.h"
namespace fovris {

class QlDeserializer : public Deserializer {
    std::vector<Query> queries_;
    std::vector<Module> modules_;

  public:
    std::vector<Query> getQueries();
    std::vector<Module>&& getModules();
    void parse(const std::string &programStr);
};
} // fovris
#endif /* end of include guard: QLDESERIALIZER_H_FWVEGBMK */
