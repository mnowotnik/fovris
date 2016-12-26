#include "thirdparty/Catch/include/catch.hpp"
#include "KB/KBTerm.h"
#include "KB/KBRuleLiteral.h"
#include "KB/KBGroundTerm.h"
#include "Eval/Join.h"

#define require_and_reverse(A,B,tupA,tupB) \
    {\
        Join join(A, B); \
        REQUIRE(join.joinable(tupA,tupB)); \
        join = Join(B,A); \
        REQUIRE(join.joinable(tupB,tupA));\
    }

#define require_false_and_reverse(A,B,tupA,tupB) \
    {\
        Join join(A, B); \
        REQUIRE_FALSE(join.joinable(tupA,tupB)); \
        join = Join(B,A); \
        REQUIRE_FALSE(join.joinable(tupB,tupA));\
    }

using namespace fovris;

TEST_CASE("Correctly identifies tuples as joinable", "[Join]") {
    using TermArray = Array<KBGroundTerm>;
    unsigned relId = 0;
    KBVarTerm XA{1},YA{2},ZA{3};
    KBVarTerm XB{4},YB{5},ZB{6};

    KBGroundTerm T1{1}, T2{2}, T3{3}, T4{4}, T5{5}, T6{6};
        
    SECTION("Relations with single common attribute"){
        KBRuleLiteral relA{relId, {XA, YA, ZA}, TruthValue::True};
        KBRuleLiteral relB{relId, {XB, YA, ZB}, TruthValue::True};

        TermArray tupA{T1,T2,T3};
        TermArray tupB{T4,T2,T5};

        require_and_reverse(relA,relB,tupA,tupB);

        tupA = {T1,T3,T3};
        tupB = {T4,T2,T5};

        require_false_and_reverse(relA,relB,tupA,tupB);
    }

    SECTION("Relations with single common attribute at different indices"){
        KBRuleLiteral relA{relId, {XA, YA, ZA}, TruthValue::True};
        KBRuleLiteral relB{relId, {YA, XB, ZB}, TruthValue::True};
        Join join(relA, relB);
        TermArray tupA{T1,T2,T3};
        TermArray tupB{T2,T4,T5};

        require_and_reverse(relA,relB,tupA,tupB);

        tupA = {T1,T2,T3};
        tupB = {T4,T2,T5};
        require_false_and_reverse(relA,relB,tupA,tupB);
    }

    SECTION("2 attributes in one relation mapped to 1 in the other"){
        KBRuleLiteral relA{relId, {XA, YA, ZA}, TruthValue::True};
        KBRuleLiteral relB{relId, {YA, YA, ZB}, TruthValue::True};
        Join join(relA, relB);

        TermArray tupA{T1,T2,T3};
        TermArray tupB{T2,T2,T5};
        require_and_reverse(relA,relB,tupA,tupB);

        tupA = {T1,T3,T3};
        tupB = {T4,T2,T5};
        require_false_and_reverse(relA,relB,tupA,tupB);
    }

    SECTION("Relations with two common attributes. Constrained to one comparison"){
        KBRuleLiteral relA{relId, {XA, YA, ZA}, TruthValue::True};
        KBRuleLiteral relB{relId, {XA, YA, ZB}, TruthValue::True};

        TermArray tupA{T1,T2,T3};
        TermArray tupB{T4,T2,T5};

        Join join({relA,{1,2}},relB);
        REQUIRE(join.joinable(tupA,tupB));

        tupA = {T1,T3,T3};
        tupB = {T4,T2,T5};

        REQUIRE_FALSE(join.joinable(tupA,tupB));
    }

}

#undef require_and_reverse
#undef require_false_and_reverse
