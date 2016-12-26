#include "Eval/JoinProjection.h"
#include "thirdparty/Catch/include/catch.hpp"

using namespace fovris;

TEST_CASE("Join projection works correctly for vector"){
    KBVarTerm X{0},Y{1},Z{2};

    KBRuleLiteral kbrA{{X,Y}},kbrB{{Y,Z}};
    JoinProjection projection(kbrA,kbrB,std::vector<KBTerm>{Z,Y});
    KBGroundTerm Tx{0},Ty{10},Tz{20};
    auto result = projection.project(std::vector<KBGroundTerm>{Tx,Ty},std::vector<KBGroundTerm>{Ty,Tz});
    auto expected = std::vector<KBGroundTerm>{Tz,Ty};
    REQUIRE(result == expected);
}

TEST_CASE("Join projection works correctly for Array"){
    KBVarTerm X{0},Y{1},Z{2},C(3);

    KBRuleLiteral kbrA{{X,Y}},kbrB{{Y,Z}};
    JoinProjection projection(kbrA,kbrB,Array<KBTerm>{Z,Y});
    KBGroundTerm Tx{0},Ty{10},Tz{20};
    auto result = projection.project(Array<KBGroundTerm>{Tx,Ty},Array<KBGroundTerm>{Ty,Tz});
    auto expected = Array<KBGroundTerm>{Tz,Ty};
    REQUIRE(result == expected);
}
