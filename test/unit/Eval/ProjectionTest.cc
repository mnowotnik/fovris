#include "thirdparty/Catch/include/catch.hpp"
#include "Eval/Projection.h"
#include "Eval/TestUtils.h"
#include <initializer_list>

using namespace fovris;

TEST_CASE("Term tuples are properly projected", "[Projection]") {
    unsigned relId = 0;
    KBVarTerm X{1}, Y{2};
    KBGroundTerm Tx{42}, Ty{43};
    KBRuleLiteral kbr{relId, {X, Y, X}, TruthValue::True};
    Projection projection(kbr, Array<KBTerm>({Y, X}));
    auto groundTerms = Array<KBGroundTerm>({Tx, Ty, Tx});
    auto result = projection.project(groundTerms);
    REQUIRE(result.size() == 2);
    REQUIRE(result[0] == Ty);
    REQUIRE(result[1] == Tx);
}
