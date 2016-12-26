#include "thirdparty/Catch/include/catch.hpp"
#include "DateTime.h"

using namespace fovris;

TEST_CASE("Check date time format parsing", "[DateTime]") {

    SECTION("Date-time is correctly parsed"){
        DateTime dt;
        SECTION("Correct ISO DateTime"){
            dt = DateTime("2015-01-05 09:06:03");
        }

        SECTION("Accepted DateTime"){
            dt = DateTime("2015-1-5 9:6:3");
        }

        REQUIRE(dt.getYear() == 2015);
        REQUIRE(dt.getMonth() == 1);
        REQUIRE(dt.getDay() == 5);
        REQUIRE(dt.getHours() == 9);
        REQUIRE(dt.getMinutes() == 6);
        REQUIRE(dt.getSeconds() == 3);
    }

    SECTION("Invalid date-time input string"){
        REQUIRE_THROWS_AS(DateTime("2015-13-05 09-06:03"), DateParsingException);
        REQUIRE_THROWS_AS(DateTime("2015-13-05 06"), DateParsingException);
        REQUIRE_THROWS_AS(DateTime("2015-13-05 :06:03"), DateParsingException);
        REQUIRE_THROWS_AS(DateTime("2015-13-05"), DateParsingException);
        REQUIRE_THROWS_AS(DateTime("2015-13-05 09 06 03"), DateParsingException);
        REQUIRE_THROWS_AS(DateTime("2015-13-05 09 06 03"), DateParsingException);
        REQUIRE_THROWS_AS(DateTime("2015-13-05 090:06:03"), DateParsingException);
        REQUIRE_THROWS_AS(DateTime("2015-13-05 09:060:03"), DateParsingException);
        REQUIRE_THROWS_AS(DateTime("2015-13-05 09:06:030"), DateParsingException);

        REQUIRE_THROWS_AS(DateTime("2015-13-05 09:06:03"), DateValidationException);
        REQUIRE_NOTHROW(DateTime("2015-12-05 00:06:03"));
        REQUIRE_THROWS_AS(DateTime("2015-12-05 24:06:03"), TimeValidationException);
        REQUIRE_THROWS_AS(DateTime("2015-12-05 12:61:03"), TimeValidationException);
        REQUIRE_THROWS_AS(DateTime("2015-12-05 12:60:61"), TimeValidationException);
        REQUIRE_THROWS_AS(DateTime("2019-02-29 09:06:03"), DateValidationException);
        REQUIRE_NOTHROW(DateTime("2020-02-29 09:06:03"));
        REQUIRE_NOTHROW(DateTime("2016-02-20 20:31"));
    }

    SECTION("Comparison operators work properly"){
        REQUIRE(DateTime("2015-12-05 23:06:03") == DateTime("2015-12-05 23:06:03"));
        std::pair<DateTime,DateTime> pairs[] = {
            std::make_pair(DateTime("2015-12-05 23:06:02"), DateTime("2015-12-05 23:06:03")),
            std::make_pair(DateTime("2015-12-05 23:05:03"), DateTime("2015-12-05 23:06:03")),
            std::make_pair(DateTime("2015-12-05 22:06:03"), DateTime("2015-12-05 23:06:03")),
            std::make_pair(DateTime("2015-12-04 23:06:03"), DateTime("2015-12-05 23:06:03")),
            std::make_pair(DateTime("2015-11-05 23:06:03"), DateTime("2015-12-05 23:06:03")),
            std::make_pair(DateTime("2014-12-05 23:06:03"), DateTime("2015-12-05 23:06:03"))
        };

        for(auto& pair: pairs){
            REQUIRE(pair.first < pair.second);
            REQUIRE(pair.second > pair.first);
            REQUIRE(pair.second != pair.first);
            REQUIRE(pair.second != pair.first);
            REQUIRE_FALSE(pair.second == pair.first);
        }
    }
}



TEST_CASE("Check date format parsing", "[Date]") {
    SECTION("Date is correctly parsed"){
        Date dt;

        SECTION("Correct ISO DateTime"){
            dt = Date("2015-01-05");
        }

        SECTION("Accepted DateTime"){
            dt = Date("2015-1-5");
        }

        REQUIRE(dt.getYear() == 2015);
        REQUIRE(dt.getMonth() == 1);
        REQUIRE(dt.getDay() == 5);
    }
    SECTION("Invalid date input string"){
        REQUIRE_THROWS_AS(Date("201-13-05"), DateParsingException);
        REQUIRE_THROWS_AS(Date("2015--05"), DateParsingException);
        REQUIRE_THROWS_AS(Date("2015-13-"), DateParsingException);
        REQUIRE_THROWS_AS(Date("2015-13"), DateParsingException);

        REQUIRE_THROWS_AS(Date("2015-13-05"), DateValidationException);
        REQUIRE_THROWS_AS(Date("2015-12-0"), DateValidationException);
        REQUIRE_THROWS_AS(Date("2019-02-29"), DateValidationException);
        REQUIRE_NOTHROW(Date("2020-02-29"));
    }

    SECTION("Comparison operators work properly"){
        REQUIRE(Date("2015-12-05") == Date("2015-12-05"));
        std::pair<Date,Date> pairs[] = {
            std::make_pair(Date("2014-12-05"), Date("2015-12-05")),
            std::make_pair(Date("2015-12-04"), Date("2015-12-05")),
            std::make_pair(Date("2015-11-05"), Date("2015-12-05")),
        };

        for(auto& pair: pairs){
            REQUIRE(pair.first < pair.second);
            REQUIRE(pair.second > pair.first);
            REQUIRE(pair.second != pair.first);
            REQUIRE(pair.second != pair.first);
            REQUIRE_FALSE(pair.second == pair.first);
        }
    }

}
