#ifndef QUERYRESULTPRINTER_H_SIHNDB9U
#define QUERYRESULTPRINTER_H_SIHNDB9U
#include "QueryResult.h"
namespace fovris {
class QueryResultPrinter {

    class Printer {
      protected:
        QueryResult &result;

      public:
        explicit Printer(QueryResult &r) : result(r) {}
    };

  public:
    class Csv : public Printer {
        using Printer::Printer;
        friend std::ostream &operator<<(std::ostream &os, const Csv &q);
    };

    class Ql : public Printer {
        using Printer::Printer;
        friend std::ostream &operator<<(std::ostream &os, const Ql &q);
    };

    class Json : public Printer {
        const int indMult = 4;
        int baseInd;
        std::string level(int l) const {
            return std::string(l * indMult + baseInd, ' ');
        }

      public:
        Json(QueryResult &r, int baseInd = 0) : Printer(r), baseInd(baseInd) {}

        friend std::ostream &operator<<(std::ostream &os, const Json &q);
    };
};

} // fovris
#endif /* end of include guard: QUERYRESULTPRINTER_H_SIHNDB9U */
