#include "KB/KBHeadLiteral.h"
#include "Eval/EvalSubgoal.h"
#include "Eval/TestUtils.h"

#include "thirdparty/Catch/include/catch.hpp"

#include <string>


using namespace fovris;

TEST_CASE("Check subgoal evaluation result", "[EvalSubgoal]") {

    // given
    unsigned relId = 0xDEADBEEF;
    KBRelation relation{
        relId, {TermType::Integer, TermType::Integer, TermType::Integer}};

    KBGroundTerm Id1{10}, Id2{20}, Id3{30};
    KBTuple tup{{Id1, Id2, Id1}, TruthValue::True};
    KBTuple tup2{{Id1, Id2, Id3}, TruthValue::True};
    relation.addTuple(tup);
    relation.addTuple(tup2);

    KBVarTerm X{1}, Y{2}, Z{5};

    KBRuleLiteral kbr{relId, {X, Y, X}, TruthValue::True};

    SECTION("Check EvalSubgoalFirst") {
        auto evalFirst = CreateEvalSubgoalFirst(kbr, EvalDestination{Y, X});
        // then
        auto result = evalFirst->evaluate({}, relation.getFacts());
        REQUIRE(result.size() == 1);
        REQUIRE(result[0].getTerms().length() == 2);
        REQUIRE(result[0].getTerms()[0] == Id2);
        REQUIRE(result[0].getTerms()[1] == Id1);
    }

    SECTION("Check EvalSubgoalNext") {
        KBRuleLiteral tmpKbr{relId, {X, Y, Z}, TruthValue::True};
        auto evalNext = CreateEvalSubgoalNext(tmpKbr, kbr, {Y, Z});
        // given

        // then
        KBTuple tmpTup = {{Id1, Id2, Id3}, TruthValue::True};
        auto result = evalNext->evaluate({tmpTup}, relation.getFacts());
        REQUIRE(result.size() == 1);
        REQUIRE(result[0].getTerms().length() == 2);
        REQUIRE(result[0].getTerms()[0] == Id2);
        REQUIRE(result[0].getTerms()[1] == Id3);
    }
}

TEST_CASE("Check subgoal with function evaluation", "[EvalSubgoal]") {
    unsigned relId = 0;
    TermMapper termMapper;

    KBGroundTerm foo{termMapper.internTerm("foo")};
    KBGroundTerm bar{termMapper.internTerm("bar")};
    KBGroundTerm baz{termMapper.internTerm("baz")};

    KBRelation relation{relId, {TermType::Id, TermType::Id, TermType::Id}};

    relation.addFact({foo, bar, baz}, TruthValue::True);
    relation.addFact({foo, foo, baz}, TruthValue::True);


    KBVarTerm X{1}, Y{2}, Z{5};
    KBRuleLiteral kbr{relId, {X, Y, Z}, TruthValue::True};

    // function predicates matche
    std::vector<KBRuleFunction> functions;
    functions.emplace_back([](const Array<Term> &terms) -> bool {
                               return terms[0].get<std::string>() == "foo" &&
                                      terms[1].get<std::string>() == "bar";
                           },
                           std::vector<KBTerm>{X, Y},
                           std::vector<TermType>{TermType::Id, TermType::Id});


    KBRuleLiteral tmpKbr{relId, {X, Y, Z}, TruthValue::True};

    auto evalNext =
        CreateEvalSubgoalLast(tmpKbr, kbr, {Y, Z}, functions, termMapper);

    KBTuple tmpTup = {{foo, bar, baz}, TruthValue::True};
    auto result = evalNext->evaluate({tmpTup}, relation.getFacts());
    REQUIRE(result.size() == 1);

    // function predicates don't match
    functions.emplace_back([](const Array<Term> &terms) -> bool {
                               return terms[0].get<std::string>() == "baz";
                           },
                           std::vector<KBTerm>{X, Y},
                           std::vector<TermType>{TermType::Id, TermType::Id});

    evalNext =
        CreateEvalSubgoalLast(tmpKbr, kbr, {Y, Z}, functions, termMapper);
    result = evalNext->evaluate({tmpTup}, relation.getFacts());
    REQUIRE(result.size() == 0);
}
