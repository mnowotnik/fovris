#include "thirdparty/Catch/include/catch.hpp"
#include "Eval/EvalRule.h"
#include "KB/KBHeadLiteral.h"
#include "Eval/TestUtils.h"

using namespace fovris;
using RelationRefVector = std::vector<std::reference_wrapper<const FactsPool>>;


TEST_CASE("Check the results of rule evaluation",
          "[Rule without disjunction]") {
    unsigned resultRel = 0;
    KBVarTerm X{1}, Y{2}, Z{5};
    auto head = KBHeadLiteral(resultRel, {Y, X});

    KBGroundTerm Id1{10}, Id2{20}, Id3{30};
    unsigned relA = 1;
    KBRelation relationA{relA, {TermType::Id, TermType::Id, TermType::Id}};

    KBTuple tup{{Id1, Id2, Id1}, TruthValue::True};
    KBTuple tup2{{Id1, Id2, Id3}, TruthValue::True};
    relationA.addTuple(tup);

    KBRuleLiteral kbr{relA, {X, Y, X}, TruthValue::True};

    SECTION("Rule with 1 subgoal") {

        RelationRefVector relationRefs;
        relationRefs.push_back(std::cref(relationA.getFacts()));

        KBRule rule{head, {kbr}};
        auto evalRuleFunc = CreateEvalRule(rule);
        auto results = (*evalRuleFunc)(relationRefs);

        REQUIRE(results.size() == 1);
        REQUIRE(results[0].getTerms()[0] == Id2);
        REQUIRE(results[0].getTerms()[1] == Id1);
        REQUIRE(results[0].getValue() == TruthValue::True);
    }

    SECTION("Rule with 2 subgoals") {

        RelationRefVector relationRefs;
        relationRefs.push_back(std::cref(relationA.getFacts()));
        relationRefs.push_back(std::cref(relationA.getFacts()));

        KBRule rule{{resultRel,{Y}}, {{relA,{X,Y,Z},TruthValue::True},{relA,{X,Y,X},TruthValue::True}}};
        auto evalRuleFunc = CreateEvalRule(rule);
        auto results = (*evalRuleFunc)(relationRefs);

        REQUIRE(results.size() == 1);
        REQUIRE(results[0].getTerms()[0] == Id2);
        REQUIRE(results[0].getValue() == TruthValue::True);
    }

    SECTION("Rule with 2 subgoals and ground term") {

        RelationRefVector relationRefs;
        relationRefs.push_back(std::cref(relationA.getFacts()));
        relationRefs.push_back(std::cref(relationA.getFacts()));

        KBRule rule{{resultRel,{KBTerm{Id1.get()},Y}}, {{relA,{X,Y,Z},TruthValue::True},{relA,{X,Y,X},TruthValue::True}}};
        auto evalRuleFunc = CreateEvalRule(rule);
        auto results = (*evalRuleFunc)(relationRefs);

        REQUIRE(results.size() == 1);
        REQUIRE(results[0].getTerms().size() == 2);
        REQUIRE(results[0].getTerms()[0] == Id1);
        REQUIRE(results[0].getTerms()[1] == Id2);
        REQUIRE(results[0].getValue() == TruthValue::True);
    }

    SECTION("Rule with negation"){
        RelationRefVector relationRefs;
        relationRefs.push_back(std::cref(relationA.getFacts()));

        KBRule rule{KBHeadLiteral{false,resultRel,{Y,X}}, {kbr}};
        auto evalRuleFunc = CreateEvalRule(rule);
        auto results = (*evalRuleFunc)(relationRefs);

        REQUIRE(results.size() == 1);
        REQUIRE(results[0].getTerms()[0] == Id2);
        REQUIRE(results[0].getTerms()[1] == Id1);
        REQUIRE(results[0].getValue() == TruthValue::False);
    }


    SECTION("Rule with 1 subgoal and custom predicate") {

        RelationRefVector relationRefs;
        relationRefs.push_back(std::cref(relationA.getFacts()));
    }
}
TEST_CASE("Test disjunction in rule", "[Rule with disjunction]") {

    unsigned resultRel = 0;
    KBVarTerm X{1}, Y{2}, Z{5}, V{6};

    KBGroundTerm Id1{10}, Id2{20}, Id3{30}, Id4{40}, Id5{50}, Id6{60};
    unsigned relA = 1;
    KBRelation relationA{relA, {TermType::Id, TermType::Id}};
    unsigned relB = 2;
    KBRelation relationB{relB, {TermType::Id, TermType::Id}};

    relationA.addFact({Id1, Id1}, TruthValue::True);
    relationA.addFact({Id1, Id2}, TruthValue::Incons);

    relationB.addFact({Id1, Id1}, TruthValue::True);
    relationB.addFact({Id1, Id2}, TruthValue::Incons);
    relationB.addFact({Id2, Id2}, TruthValue::Incons);

    // cartesian product (except for Id2,Id2)
    KBRuleLiteral kbrA{relA, {X, Z}, {TruthValue::True, TruthValue::Incons}};
    KBRuleLiteral kbrB{relB, {X, V}, {TruthValue::True, TruthValue::Incons}};
    KBRule rule{{resultRel, {X}},
                {KBRuleDisjunct{{kbrA}}, KBRuleDisjunct{{kbrB}}}};
    auto evalRuleFunc = CreateEvalRule(rule);

    RelationRefVector relationRefs;
    relationRefs.push_back(std::cref(relationA.getFacts()));
    relationRefs.push_back(std::cref(relationB.getFacts()));
    auto results = (*evalRuleFunc)(relationRefs);

    std::set<KBTuple> resultSet;

    for(auto& resultTup : results){
        resultSet.insert(resultTup);
    }

    REQUIRE(resultSet.size() == 3);
}
