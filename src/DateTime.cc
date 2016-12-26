#include "DateTime.h"

#include "boost/functional/hash/hash.hpp"

#include <ostream>
namespace fovris {

namespace {

const std::regex DatePattern("(\\d{4})-(\\d{1,2})-(\\d{1,2})");
const std::regex DateTimePattern(
    "(\\d{4})-(\\d{1,2})-(\\d{1,2})[T ](\\d{1,2}):(\\d{1,2})(?::(\\d{1,2}))?");

// source : http://linux.die.net/man/3/timegm
time_t my_timegm(struct tm *tm) {
    time_t ret;
    char *tz;

    tz = getenv("TZ");
    setenv("TZ", "", 1);
    tzset();
    ret = mktime(tm);
    if (tz)
        setenv("TZ", tz, 1);
    else
        unsetenv("TZ");
    tzset();
    return ret;
}

void validateDate(int year, unsigned month, unsigned day) {
    auto ymd = date::year_month_day{date::year{year}, date::month{month},
                                    date::day{day}};
    if (!ymd.ok()) {
        throw DateValidationException{
            std::string("Invalid Date: ") + std::to_string(year) + '-' +
            std::to_string(month) + '-' + std::to_string(day)};
    }
}

void validateTime(unsigned hour, unsigned minute, unsigned second) {
    if (hour >= 24 || minute > 60 || second > 60) {
        throw TimeValidationException{
            std::string("Invalid Time: ") + std::to_string(hour) + ':' +
            std::to_string(minute) + ':' + std::to_string(second)};
    }
}

std::tm makeTm(int year, int month, int day, int hour, int minute, int second) {
    validateTime(hour, minute, second);
    validateDate(year, month, day);
    std::tm tm;
    tm.tm_sec = second;
    tm.tm_min = minute;
    tm.tm_hour = hour;
    tm.tm_mday = day;
    tm.tm_mon = month - 1;
    tm.tm_year = year - 1900;
    tm.tm_isdst = -1;
    return tm;
}

std::tm parseDate(const std::string &dtStr) {
    std::smatch match;
    std::tm dt;
    if (std::regex_match(dtStr, match, DatePattern) && match.size() == 4) {
        return makeTm(std::stoi(match.str(1)), std::stoi(match.str(2)),
                      std::stoi(match.str(3)), 0, 0, 0);
        return dt;
    }
    throw DateParsingException{"Cannot parse date: " + dtStr};
}

std::tm parseDateTimeStr(const std::string &dtStr) {
    std::smatch match;
    std::tm dt;
    std::regex_match(dtStr, match, DateTimePattern);
    if (match.size() != 7) {
        throw DateParsingException{"Cannot parse datetime: " + dtStr};
    }

    int second = 0;
    if (!match.str(6).empty()) {
        second = std::stoi(match.str(6));
    }
    return makeTm(std::stoi(match.str(1)), std::stoi(match.str(2)),
                  std::stoi(match.str(3)), std::stoi(match.str(4)),
                  std::stoi(match.str(5)), second);
    return dt;
}

// FIXME TODO: my_timegm is NOT threadsafe. Change to threadsafe version
std::chrono::system_clock::time_point tmToTimePoint(std::tm tm) {
    return std::chrono::system_clock::from_time_t(my_timegm(&tm));
}

template <class T>
std::ostream &addLeadingZero(std::ostream &os, const T &val) {
    if (static_cast<unsigned>(val) < 10) {
        os << 0;
    }
    os << val;
    return os;
}
} // namespace


Date::Date(const std::string &dtStr) : Date(tmToTimePoint(parseDate(dtStr))) {}
Date::Date(unsigned year, unsigned month, unsigned day)
    : Date(tmToTimePoint(makeTm(year, month, day, 0, 0, 0))) {
    validateDate(year, month, day);
}

date::year_month_day Date::getYearMonthDay() const {
    auto dp = date::floor<date::days>(tp);
    return date::year_month_day{dp};
}

unsigned Date::getYear() const {
    return static_cast<int>(getYearMonthDay().year());
}
unsigned Date::getMonth() const {
    return static_cast<unsigned>(getYearMonthDay().month());
}
unsigned Date::getDay() const {
    return static_cast<unsigned>(getYearMonthDay().day());
}


std::ostream &operator<<(std::ostream &os, const Date &d) {
    auto ymd = d.getYearMonthDay();
    os << static_cast<int>(ymd.year()) << '-';

    addLeadingZero(os, static_cast<unsigned>(ymd.month())) << '-';
    addLeadingZero(os, static_cast<unsigned>(ymd.day()));

    return os;
}

bool Date::operator<(const Date &other) const { return tp < other.tp; }
bool Date::operator>(const Date &other) const { return tp > other.tp; }
bool Date::operator==(const Date &other) const { return tp == other.tp; }
bool Date::operator!=(const Date &other) const { return tp != other.tp; }

DateTime::DateTime(const std::string &dtStr)
    : Date(tmToTimePoint(parseDateTimeStr(dtStr))) {}

DateTime::DateTime(unsigned year, unsigned month, unsigned day, unsigned hour,
                   unsigned minute, unsigned second)
    : Date(tmToTimePoint(makeTm(year, month, day, hour, minute, second))) {}

date::time_of_day<std::chrono::seconds> DateTime::getTimeOfDay() const {
    auto dp = date::floor<date::days>(tp);
    auto time = date::make_time(
        std::chrono::duration_cast<std::chrono::seconds>(tp - dp));
    time.make24();
    return time;
}

unsigned DateTime::getHours() const {
    using namespace date;
    return getTimeOfDay().hours().count();
}
unsigned DateTime::getMinutes() const {
    return getTimeOfDay().minutes().count();
}
unsigned DateTime::getSeconds() const {
    return getTimeOfDay().seconds().count();
}


std::ostream &operator<<(std::ostream &os, const DateTime &dt) {
    auto tod = dt.getTimeOfDay();
    os << static_cast<const Date &>(dt) << ' ';

    addLeadingZero(os, tod.hours().count()) << ':';
    addLeadingZero(os, tod.minutes().count()) << ':';
    addLeadingZero(os, tod.seconds().count());

    return os;
}

bool DateTime::operator<(const DateTime &other) const { return tp < other.tp; }
bool DateTime::operator>(const DateTime &other) const { return tp > other.tp; }
bool DateTime::operator==(const DateTime &other) const {
    return tp == other.tp;
}
bool DateTime::operator!=(const DateTime &other) const {
    return tp != other.tp;
}
} // fovris


namespace std {
using namespace fovris;
using chrono::system_clock;
size_t inline hash_ymd(const date::year_month_day &ymd) {
    size_t seed = 0;
    boost::hash_combine(seed, static_cast<int>(ymd.year()));
    boost::hash_combine(seed, static_cast<unsigned>(ymd.month()));
    boost::hash_combine(seed, static_cast<unsigned>(ymd.day()));
    return seed;
}

size_t hash<Date>::operator()(const Date &d) const {
    return hash_ymd(d.getYearMonthDay());
}
size_t hash<DateTime>::operator()(const DateTime &dt) const {
    auto ymd = dt.getYearMonthDay();
    size_t seed = hash_ymd(ymd);
    auto tod = dt.getTimeOfDay();
    boost::hash_combine(seed, tod.hours().count());
    boost::hash_combine(seed, tod.minutes().count());
    boost::hash_combine(seed, tod.seconds().count());
    return seed;
}
} // std
