#ifndef QUERYRESULT_H_HN061JBU
#define QUERYRESULT_H_HN061JBU
#include <vector>

#include "Query.h"
#include "ResultFact.h"
namespace fovris {

class QueryResult {
  public:
    typedef std::vector<ResultFact>::const_iterator const_iterator;
    typedef std::vector<ResultFact>::size_type size_type;

    QueryResult(Query query, std::vector<ResultFact> facts,
                int termsLength = -1)
        : facts(std::move(facts))
        , query(std::move(query))
        , termsLength(termsLength) {}


    const ResultFact &operator[](size_type i) const { return facts[i]; }

    const ResultFact &front() const { return facts.front(); }

    const ResultFact &back() const { return facts.back(); }

    size_type size() const { return facts.size(); }

    const_iterator begin() const { return facts.begin(); }
    const_iterator end() const { return facts.end(); }

    int getTermsLength() { return termsLength; }
    const Query &getQuery() const { return query; }

    friend std::ostream &operator<<(std::ostream &os, const QueryResult &q);
    std::ostream &print(std::ostream &os, int indent) const;

  private:
    std::vector<ResultFact> facts;
    Query query;
    int termsLength;
};
} // fovris
#endif /* end of include guard: QUERYRESULT_H_HN061JBU */
