#include "fovris/Program.h"
#include "fovris/QlDeserializer.h"
#include "fovris/QueryResultPrinter.h"

#include "crow.h"
#include <boost/algorithm/string.hpp>

#include <algorithm>
#include <ostream>
#include <string>
#include <vector>

std::string makeErrorMessage(const fovris::ParsingError &e) {
    return "Error at line  " + std::to_string(e.getLine()) + " ,position " +
           std::to_string(e.getPos());
}

std::string makeQueryErrorMessage(const fovris::InvalidInputException &e,
                                  const std::string &query) {
    return "Error in query: " + query + " . Reason: " + e.what();
}

void printHelp() {
    std::cout << "Usage: fovrisd [ADDR] [PORT]\n";
    std::cout << "Minimal stateless server that accepts 4QL scripts\n";
    std::cout << "and responds with results.\n\n";
    std::cout << "How to use:\n";
    std::cout << "Send POST request to the /query endpoint with 4QL\n";
    std::cout << "script in the body.\n";
}

const std::vector<std::string> accessControlRequestMethods = {"GET", "POST",
                                                              "PUT", "DELETE"};

crow::response queryOptions(const crow::request &req) {
    crow::response resp;
    if (req.get_header_value("Access-Control-Request-Method") == "") {
        resp.code = 400;
        resp.body = "Invalid preflight request";
        return resp;
    }

    const std::string reqMethod =
        req.get_header_value("Access-Control-Request-Method");

    if (!std::any_of(std::begin(accessControlRequestMethods),
                     std::end(accessControlRequestMethods),
                     std::bind(std::equal_to<std::string>(), reqMethod,
                               std::placeholders::_1))) {
        resp.code = 400;
        resp.body = "Invalid preflight request";
        return resp;
    }

    resp.set_header("Access-Control-Allow-Methods", "GET, POST");
    resp.set_header("Access-Control-Allow-Origin", "*");
    resp.set_header("Access-Control-Allow-Headers", "Access-Control-Allow-Methods, Access-Control-Allow-Origin");
    return resp;
}

crow::response queryPost(const crow::request &req) {
    fovris::QlDeserializer sp;
    crow::response resp;
    resp.set_header("Access-Control-Allow-Origin", "*");
    try {
        sp.parse(req.body);
    } catch (fovris::ParsingError &e) {
        resp.body = makeErrorMessage(e);
        resp.code = 400;
        return resp;
    }
    fovris::Program program(fovris::Algorithm::Seminaive);
    for (auto &module : sp.getModules()) {
        program.addModule(module);
    }
    program.evaluate();

    for (auto &query : sp.getQueries()) {
        std::ostringstream ss;
        try {
            std::string resultStr = fovris::print(
                fovris::QueryResultPrinter::Ql(program.query(query)));
            resp.write(resultStr);
        } catch (fovris::InvalidInputException &e) {
            resp.body = makeQueryErrorMessage(e, fovris::print(query));
            resp.code = 400;
            return resp;
        }
        resp.write("\n");
    }

    if (sp.getQueries().empty()) {
        resp.write("Result is empty.");
    }
    return resp;
}

int main(int argc, char **argv) {

    if (argc != 3 && argc != 1) {
        printHelp();
        return 1;
    }

    std::string addr = "0.0.0.0";
    int port = 8080;

    if (argc == 3) {
        addr = std::string(argv[1]);
        port = std::stoi(argv[2]);
    }

    crow::SimpleApp app;

    CROW_ROUTE(app, "/query")
        .methods("OPTIONS"_method, "POST"_method)([](const crow::request &req) {
            if (req.method == "POST"_method) {
                return queryPost(req);
            } 
            return queryOptions(req);
        });

    CROW_ROUTE(app, "/healthz")
        .methods("GET"_method)([](const crow::request &req) {
            return "ok";
        });

    CROW_LOG_INFO << "Starting server: " << addr << ":" << port;
    app.bindaddr(addr).port(port).multithreaded().run();
}
