#ifndef DATAMAPPER_H_ZIDLKRYB
#define DATAMAPPER_H_ZIDLKRYB

#include "DataPool.h"
#include "DateTime.h"
#include "Term.h"
#include "KB/KBGroundTerm.h"
#include "KB/KBTerm.h"
namespace fovris {

class TermMapper {
    DataPool<double> dblTermPool_;
    DataPool<Date> dateTermPool_;
    DataPool<DateTime> dateTimeTermPool_;
    DataPool<std::string> strTermPool_;

  public:
    unsigned internTerm(const Term &term);

    unsigned queryTermId(const Term &term) const;

    Term queryTerm(unsigned id, TermType type) const;
};
} // fovris
#endif /* end of include guard: DATAMAPPER_H_ZIDLKRYB */
