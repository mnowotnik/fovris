#include "QueryResultPrinter.h"
namespace fovris {

namespace {
std::regex quoteRegex("\"");
std::string escapeQuotes(const std::string &str) {
    return std::regex_replace(str, quoteRegex, "\"\"");
}

bool hasQuotes(const std::string &str) {
    return str.find('"') != std::string::npos;
}
} // namespace

std::ostream &operator<<(std::ostream &os, const QueryResultPrinter::Csv &q) {
    const QueryResult &result = q.result;

    os << '#' << result.getQuery() << std::endl;

    for (auto &resFact : result) {

        for (auto &term : resFact) {
            if (term.getType() == TermType::String &&
                hasQuotes(term.get<const std::string &>())) {
                os << '\"' << escapeQuotes(term.get<const std::string &>())
                   << '\"' << ',';
            } else {
                os << term << ',';
            }
        }

        os << resFact.value << std::endl;
    }

    return os;
}
std::ostream &operator<<(std::ostream &os, const QueryResultPrinter::Ql &q) {
    const QueryResult &result = q.result;
    os << '#' << result.getQuery();
    os << std::endl;
    os << result;
    return os;
}

std::ostream &operator<<(std::ostream &os, const QueryResultPrinter::Json &q) {
    const QueryResult &result = q.result;

    os << q.level(0) << '{' << std::endl;
    os << q.level(1) << "\"query\": \"" << result.getQuery() << "\","
       << std::endl;
    os << q.level(1) << "\"facts\": [" << std::endl;

    int i = 0;
    for (auto &resFact : result) {

        if (i > 0) {
            os << q.level(1) << "},{" << std::endl;
        } else {
            os << q.level(1) << '{' << std::endl;
        }

        os << q.level(2) << "\"terms\": [";

        int j = 0;
        for (auto &term : resFact) {

            if (j > 0) {
                os << ',';
            }

            if (term.getType() == TermType::String) {
                os << term;
            } else {
                os << '"' << term << '"';
            }

            j++;
        }

        os << "]," << std::endl;
        os << q.level(2) << "\"value\": " << '"' << resFact.value << '"'
           << std::endl;

        i++;
    }

    os << q.level(1) << "}]" << std::endl;

    os << q.level(0) << "}";
    return os;
}
} // fovris
