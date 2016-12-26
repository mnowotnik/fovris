#ifndef DATETIME_H_XVNSDC9H
#define DATETIME_H_XVNSDC9H
#include <chrono>
#include <ctime>
#include <functional>
#include <regex>
#include <stdexcept>
#include <string>

#include "thirdparty/date/date.h"

namespace fovris {
class Date;
class DateTime;
}

namespace std {
template <> struct hash<fovris::DateTime> {
    size_t operator()(const fovris::DateTime &dt) const;
};
template <> struct hash<fovris::Date> {
    size_t operator()(const fovris::Date &d) const;
};
} // std

namespace fovris {

class DateValidationException : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class TimeValidationException : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class DateTimeParsingException : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class DateParsingException : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class Date {
  protected:
    std::chrono::system_clock::time_point tp;

    Date(std::chrono::system_clock::time_point tp) : tp(tp) {}

  public:
    Date() {}
    Date(const std::string &dtStr);
    Date(unsigned year, unsigned month, unsigned day);

    bool operator<(const Date &other) const;
    bool operator>(const Date &other) const;
    bool operator==(const Date &other) const;
    bool operator!=(const Date &other) const;

    date::year_month_day getYearMonthDay() const;

    unsigned getYear() const;
    unsigned getMonth() const;
    unsigned getDay() const;

    friend std::ostream &operator<<(std::ostream &os, const Date &t);

    friend size_t std::hash<fovris::Date>::operator()(const Date &d) const;
};

// Warning: The class constructor is NOT threadsafe!
class DateTime : public Date {
  public:
    DateTime() : Date() {}
    DateTime(const std::string &dtStr);
    DateTime(unsigned year, unsigned month, unsigned day, unsigned hour,
             unsigned minute, unsigned second);

    bool operator<(const DateTime &other) const;
    bool operator>(const DateTime &other) const;
    bool operator==(const DateTime &other) const;
    bool operator!=(const DateTime &other) const;

    date::time_of_day<std::chrono::seconds> getTimeOfDay() const;

    unsigned getHours() const;
    unsigned getMinutes() const;
    unsigned getSeconds() const;

    friend std::ostream &operator<<(std::ostream &os, const DateTime &t);
    friend size_t std::hash<fovris::DateTime>::operator()(const DateTime &dt) const;
};
} // fovris


#endif /* end of include guard: DATETIME_H_XVNSDC9H */
