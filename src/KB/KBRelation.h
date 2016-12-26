#ifndef KBRELATION_H_2OTQM5TU
#define KBRELATION_H_2OTQM5TU

#include "Array.h"
#include "KB/KBGroundLiteral.h"
#include "KB/KBTerm.h"
#include "KB/KBTuple.h"
#include "Term.h"

#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index_container.hpp>

namespace fovris {

std::size_t hash_value(const Array<KBGroundTerm> &terms);

using FactsPool = boost::multi_index::multi_index_container<
    KBTuple,
    boost::multi_index::indexed_by<
        boost::multi_index::random_access<>,
        boost::multi_index::hashed_unique<boost::multi_index::const_mem_fun<
            KBTuple, const Array<KBGroundTerm> &, &KBTuple::getTerms>>>>;

struct KBRelation {
  private:
    FactsPool facts;
    FactsPool edbFacts;
    unsigned inconsFactsNumber_ = 0;
    unsigned edbInconsFactsNumber_ = 0;
    bool finalized_ = false;

  public:
    KBRelation(unsigned id, std::vector<TermType> colDomains);

    const unsigned id;
    const std::vector<TermType> colDomains;

    void addFact(const KBGroundLiteral &literal);
    void addEdbFact(const KBGroundLiteral &literal);
    void addFact(const Array<KBGroundTerm> &terms, TruthValue value);
    void addFact(Array<KBGroundTerm> &&terms, TruthValue value);

    template <typename T> std::pair<bool, TruthValue> addTuple(T &&tuple) {
        auto &raIdx = facts.get<0>();
        auto retIt = raIdx.push_back(std::forward<T>(tuple));
        if (!retIt.second) {
            TruthValue prevValue = retIt.first->getValue();
            TruthValue newValue =
                DisjunctionWrtOrdering(prevValue, tuple.getValue());

            if (prevValue == newValue) {
                return std::make_pair(false, prevValue);
            }

            raIdx.modify(retIt.first,
                         [newValue](KBTuple &tup) { tup.setValue(newValue); });
            if (prevValue != TruthValue::Incons &&
                newValue == TruthValue::Incons) {
                inconsFactsNumber_++;
                return std::make_pair(true, TruthValue::Incons);
            }

            return std::make_pair(false, prevValue);
        } else {
            return std::make_pair(true, tuple.getValue());
        }
    }

    template <typename T> void addEdbTuple(T &&tuple) {
        if (finalized_) {
            return;
        }

        auto &raIdx = edbFacts.get<0>();
        auto retIt = raIdx.push_back(std::forward<T>(tuple));
        if (!retIt.second) {
            TruthValue prevValue = retIt.first->getValue();
            TruthValue newValue =
                DisjunctionWrtOrdering(prevValue, tuple.getValue());

            if (prevValue == newValue) {
                return;
            }

            if (prevValue != TruthValue::Incons &&
                newValue == TruthValue::Incons) {
                edbInconsFactsNumber_++;
            }

            raIdx.modify(retIt.first,
                         [newValue](KBTuple &tup) { tup.setValue(newValue); });
        }
    }

    void finalize();
    void clearIncons(const FactsPool &removed);
    void addIncons(const FactsPool &added);
    void clear();
    bool remove(const KBTuple &tuple);

    unsigned inconsFactsNumber() const;
    unsigned edbInconsFactsNumber() const;
    const FactsPool &getFacts() const;
    const FactsPool &getEdbFacts() const;
    unsigned size() const;
    unsigned edbSize() const;

    FactsPool &getFacts();
    FactsPool &getEdbFacts();
};

} // fovris
#endif /* end of include guard: KBRELATION_H_2OTQM5TU */
