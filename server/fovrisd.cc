#include "crow.h"
#include "fovris/Program.h"
#include "fovris/QlDeserializer.h"
#include "fovris/QueryResultPrinter.h"

#include <ostream>
#include <string>

std::string makeErrorMessage(const fovris::ParsingError &e) {
    return "Error at line  " + std::to_string(e.getLine()) + " ,position " +
           std::to_string(e.getPos());
}

int main(int argc, char **argv) {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/query")
        .methods("POST"_method)([](const crow::request &req) {
            fovris::QlDeserializer sp;
            try {
                sp.parse(req.body);
            } catch (fovris::ParsingError &e) {
                return crow::response(400, makeErrorMessage(e));
            }
            fovris::Program program(fovris::Algorithm::Seminaive);
            for (auto &module : sp.getModules()) {
                program.addModule(module);
            }
            program.evaluate();
            crow::response resp;

            for (auto &query : sp.getQueries()) {
                std::ostringstream ss;
                try {
                    ss << fovris::QueryResultPrinter::Ql(program.query(query));
                } catch (fovris::InvalidInputException &e) {
                    return crow::response(400, e.what());
                }
                std::string resultStr = ss.str();
                resp.write(resultStr);
                resp.write("\n");
            }

            if (sp.getQueries().empty()) {
                resp.write("Result is empty.");
            }

            return resp;
        });
    app.port(8080).multithreaded().run();
}
