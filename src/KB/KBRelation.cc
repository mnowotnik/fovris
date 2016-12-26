#include "KBRelation.h"
#include "make_unique.hpp"

#include "boost/functional/hash/hash.hpp"

#include <tuple>
namespace fovris {

namespace {
struct SetUnknown {
    void operator()(KBTuple &tup) { tup.setValue(TruthValue::Unknown); }
} setUnknown;
}

// for boost::hash
std::size_t hash_value(KBGroundTerm const &term) {
    return std::hash<unsigned>()(term.get());
}


std::size_t hash_value(const Array<KBGroundTerm> &terms) {
    std::size_t seed = 0;
    boost::hash_combine(seed, boost::hash_range(terms.begin(), terms.end()));
    return seed;
}

KBRelation::KBRelation(unsigned id, std::vector<TermType> colDomains)
    : id(id), colDomains(std::move(colDomains)) {}
void KBRelation::addFact(const KBGroundLiteral &literal) {
    addTuple(KBTuple(literal.getTerms(), literal.getValue()));
}

void KBRelation::addEdbFact(const KBGroundLiteral &literal){
    addEdbTuple(KBTuple(literal.getTerms(), literal.getValue()));
}


void KBRelation::addFact(const Array<KBGroundTerm> &terms, TruthValue value) {
    addTuple(KBTuple(terms, value));
}

void KBRelation::addFact(Array<KBGroundTerm> &&terms, TruthValue value) {
    addTuple(KBTuple(std::move(terms), value));
}

unsigned KBRelation::size() const { return facts.size(); }
unsigned KBRelation::edbSize() const { return edbFacts.size(); }

FactsPool &KBRelation::getFacts() { return facts; }
const FactsPool &KBRelation::getFacts() const { return facts; }

FactsPool &KBRelation::getEdbFacts() { return edbFacts; }
const FactsPool &KBRelation::getEdbFacts() const { return edbFacts; }

void KBRelation::finalize() {
    if (finalized_) {
        return;
    }
    facts = edbFacts;
    finalized_ = true;
    inconsFactsNumber_ = edbInconsFactsNumber_;
}

unsigned KBRelation::inconsFactsNumber() const { return inconsFactsNumber_; }
unsigned KBRelation::edbInconsFactsNumber() const {
    return edbInconsFactsNumber_;
}

void KBRelation::clearIncons(const FactsPool &removed) {
    auto &exRaIdx = removed.get<0>();
    auto &setIdx = facts.get<1>();
    auto exit = exRaIdx.begin();
    for (; exit != exRaIdx.end(); exit++) {
        if (exit->getValue() == TruthValue::Incons) {
            auto it = setIdx.find(exit->getTerms());
            if (it != setIdx.end() && it->getValue() != TruthValue::Unknown) {
                setIdx.modify(it, setUnknown);
                if(it->getValue()==TruthValue::Incons){
                    inconsFactsNumber_--;
                }
            }
        }
    }
}

void KBRelation::addIncons(const FactsPool &added) {
    for (auto &addedTup : added) {
        if (addedTup.getValue() == TruthValue::Incons) {
            addTuple(addedTup);
            inconsFactsNumber_++;
        }
    }
}

void KBRelation::clear() {
    facts = edbFacts;
    inconsFactsNumber_ = edbInconsFactsNumber_;
}

bool KBRelation::remove(const KBTuple &tuple) {
    auto &setIdx = facts.get<1>();
    auto it = setIdx.find(tuple.getTerms());
    if (it != setIdx.end()) {
        setIdx.modify(it, setUnknown);
        return true;
    }
    return false;
}

} // fovris
