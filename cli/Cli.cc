#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <cstdlib>
#include <cstring>

// third-party
#include "linenoise.h"
#include "optionparser.h"

#include "QlDeserializer.h"
#include "Utils.h"

#include "CmdLineScanner.h"
#include "Lexer.h"
#include "NaiveEvaluation.h"
#include "Program.h"
#include "QueryResultPrinter.h"
#include "SemiNaiveEvaluation.h"

#if defined(_OPENMP)
#include <omp.h>
#endif

using namespace fovris;

class NoSuchAlgorithm : public std::runtime_error {
    using runtime_error::runtime_error;
};

typedef NaiveEvaluation DefaultEvaluation;

enum OutputFormat { Ql, Csv, Json };

const std::map<std::string, OutputFormat> FormatMap{
    {"ql", Ql}, {"csv", Csv}, {"json", Json}};

void printParsingError(ParsingError &e) {
    std::cerr << "error:" << e.getLine() << ':' << e.getPos() << ": "
              << e.what() << std::endl;
}


void interactiveCli(Program &program);

struct ProgramAndQueries {
    Program program;
    std::vector<Query> queries;
};

void runProgramAndExit(const Program &program,
                       const std::vector<Query> &queries,
                       OutputFormat outputFmt);

std::vector<Query> readProgramFromFiles(const std::vector<std::string> &imports,
                                        Program &program);
std::vector<Query> readProgramFromStream(std::istream &is, Program &program);

std::ostream &operator<<(std::ostream &os, const option::Option &opt) {
    os << std::string(opt.name, opt.namelen);
    return os;
}

struct Arg : public option::Arg {
    static option::ArgStatus Unknown(const option::Option &option, bool err) {
        if (err) {
            std::cerr << "Unknown option '" << option << "'" << std::endl;
        }
        return option::ARG_ILLEGAL;
    }
    static option::ArgStatus Required(const option::Option &option, bool err) {
        if (option.arg != 0) {
            return option::ARG_OK;
        }

        if (err) {
            std::cerr << "Option '" << option << "' requires an arguments."
                      << std::endl;
        }

        return option::ARG_ILLEGAL;
    }
};

/* unsigned int nthreads = std::thread::hardware_concurrency(); */
enum OptionIndex { UNKNOWN, HELP, IMPORT, QUERY, FORMAT, ALGORITHM, THREADS ,STDIN};

Algorithm getAlgorithm(std::string alg);

const option::Descriptor usage[] = {
    {UNKNOWN, 0, "", "", option::Arg::None, ""},
    {HELP, 0, "h", "help", Arg::None,
     "  -h, --help            Print this and exit."},
    {ALGORITHM, 0, "a", "algorithm", Arg::Required,
     "  -a, --algorithm  ALG  Set the evaluation algorithm.\n"
     "                        Possible values:\n"
     "                          N - naive,\n"
     "                          SN - seminaive\n"},
    {THREADS, 0, "t", "threads", Arg::Optional,
     "  -t, --threads[NUM]    Run using NUM threads or, if NUM not defined,\n"
     "                        the number of available cores"},
    {IMPORT, 0, "i", "import", Arg::Required,
     "  -i, --import   PATH   Import a 4ql program."},
    {QUERY, 0, "q", "query", Arg::Required,
     "  -q, --query    QUERY  Print query results and exit."},
    {FORMAT, 0, "f", "format", Arg::Required,
     "  -f, --format   FORMAT Set a format for the query result.\n"
     "                        Possible values: ql,csv."},
    {STDIN, 0, "", "stdin", Arg::None,
     "  --stdin               Read script from stdin.\n"},
    {0, 0, 0, 0, 0, 0}};

int main(int argc, char **argv) {
    argc -= (argc > 0);
    argv += (argc > 0); // skip program name argv[0] if present
    option::Stats stats(usage, argc, argv);
    std::vector<option::Option> options(stats.options_max),
        buffer(stats.buffer_max);
    option::Parser parse(usage, argc, argv, options.data(), buffer.data());

    if (parse.error()) {
        std::cerr << "Error parsing command line parameters" << std::endl;
        return 1;
    }

    if (options[UNKNOWN].count() != 0 || parse.nonOptionsCount()) {
        std::cerr << "Unknown option: " << options[UNKNOWN].first()->name
                  << std::endl;
        std::cout << "Try 'fovris -h' for help" << std::endl;
        return 1;
    }

    if (options[HELP]) {
        std::cout << "Usage: fovris [options]" << std::endl << std::endl;
        std::cout << "Options:" << std::endl;
        option::printUsage(std::cout, usage);
        return 0;
    }

    if (options[FORMAT].count() > 1) {
        std::cerr << "Cannot set more than one output format." << std::endl;
        return 1;
    }

    if (options[THREADS]) {
        if (options[FORMAT].count() > 1) {
            std::cerr << "Declare --threads option only once." << std::endl;
            return 1;
        }
#if !defined(_OPENMP)
        std::cerr << "WARNING! The application was compiled without openmp "
                     "support, 'threads' flag has no effect."
                  << std::endl;
#else
        int threads = std::stoi(std::string(options[THREADS].first()->arg));
        omp_set_num_threads(threads);
#endif
    }


    std::vector<std::string> imports;

    for (option::Option *opt = options[IMPORT]; opt; opt = opt->next()) {
        imports.emplace_back(opt->arg);
    }

    OutputFormat outFmt = OutputFormat::Ql;
    if (options[FORMAT]) {
        std::string fmt = std::string(options[FORMAT].first()->arg);

        if (FormatMap.find(fmt) == FormatMap.end()) {
            std::cerr << "Invalid output format: '" << fmt << "'" << std::endl;
            return 1;
        }

        outFmt = FormatMap.at(fmt);
    }

    Algorithm algorithm;
    if (options[ALGORITHM]) {
        try {
            std::string alg = std::string(options[ALGORITHM].first()->arg);
            algorithm = getAlgorithm(alg);
        } catch (NoSuchAlgorithm e) {
            std::cerr << "No such algorithm: '" << e.what() << "'.";
            return 1;
        }

    } else {
        algorithm = Algorithm::Seminaive;
    }


    std::vector<Query> queries;
    for (option::Option *opt = options[QUERY]; opt; opt = opt->next()) {
        queries.emplace_back(opt->arg);
    }

    try {

        Program program(algorithm);
        std::vector<Query> additionalQueries;

        if (!imports.empty()) {
            additionalQueries = readProgramFromFiles(imports, program);
        } else if(options[STDIN]){
            additionalQueries = readProgramFromStream(std::cin, program);
        }

        std::move(additionalQueries.begin(), additionalQueries.end(),
                  std::back_inserter(queries));

        program.evaluate();

        if (queries.empty()) {
            interactiveCli(program);
        } else {
            runProgramAndExit(program, queries, outFmt);
        }

    } catch (FileAccessException &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (ParsingError &e) {
        printParsingError(e);
        return 1;
    } catch (InvalidInputException &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    /* for (int i = 0; i < parse.nonOptionsCount(); ++i) */
    /*     std::cout << "Non-option #" << i << ": " << parse.nonOption(i) <<
     * "\n"; */
    return 0;
}
void runProgramAndExit(const Program &program,
                       const std::vector<Query> &queries,
                       OutputFormat outputFmt) {

    if (outputFmt == OutputFormat::Json) {
        std::cout << '[';
    }

    int i = 0;
    for (auto &query : queries) {
        QueryResult result = program.query(query);
        switch (outputFmt) {
            case OutputFormat::Csv:
                std::cout << QueryResultPrinter::Csv(result);
                break;

            case OutputFormat::Ql:
                std::cout << QueryResultPrinter::Ql(result);
                break;

            case OutputFormat::Json:
                if (i > 0) {
                    std::cout << ',';
                }
                std::cout << QueryResultPrinter::Json(result);
                break;
        }
        i++;
    }
    if (outputFmt == OutputFormat::Json) {
        std::cout << ']' << std::endl;
    }
}

std::vector<Query> readProgramFromStream(std::istream &is, Program &program) {
    QlDeserializer qlD;
    std::stringstream ss;
    ss << is.rdbuf();

    qlD.parse(ss.str());
    program.addModules(qlD.getModules());

    return qlD.getQueries();
}

std::vector<Query> readProgramFromFiles(const std::vector<std::string> &imports,
                                        Program &program) {
    QlDeserializer qlD;

    std::vector<Query> queries;
    for (auto &path : imports) {
        std::string ext = fileExtension(path);
        if (ext == "4ql") {
            qlD.parse(fileToString(path));
            program.addModules(qlD.getModules());
            auto newQueries = qlD.getQueries();
            std::copy(newQueries.begin(), newQueries.end(),
                      std::back_inserter(queries));
        } else {
            throw InvalidInputException("File extension not supported: ", ext);
        }
    }
    return queries;
}

void addRelationCompletions(std::vector<std::string> &cmpls,
                            const std::vector<RelationDef> &rels,
                            const std::string &module,
                            const std::string &prefix) {
    for (auto &rel : rels) {
        if (std::equal(prefix.begin() + module.length() + 1, prefix.end(),
                       rel.getPredicateSymbol().begin())) {

            if (prefix.length() - module.length() - 1 ==
                rel.getPredicateSymbol().length()) {
                cmpls.push_back(module + "." + rel.getPredicateSymbol() + "(");
            } else {
                cmpls.push_back(module + "." + rel.getPredicateSymbol());
            }
        }
    }
}
std::vector<std::string> getModuleCompletions(Program &program,
                                              const std::string &prefix) {

    std::vector<std::string> cmpls;
    for (auto &mod : program.listModules()) {
        // prefix is shorter than module name
        if (std::equal(prefix.begin(), prefix.end(), mod.begin())) {
            if (prefix.length() == mod.length()) {
                cmpls.push_back(mod + std::string("."));
            } else {
                cmpls.push_back(mod);
            }
            // prefix is longer than module name, add relations
        } else if (std::equal(mod.begin(), mod.end(), prefix.begin()) &&
                   prefix[mod.length()] == '.') {
            addRelationCompletions(cmpls, program.listRelations(mod), mod,
                                   prefix);
        }
    }
    return cmpls;
}

const std::vector<std::string> Commands{"help", "import", "exit", "clear",
                                        "modules"};

// dynamic completion using Program state
void completion(const std::string &prefix, linenoiseCompletions *lc,
                std::reference_wrapper<Program> programRef) {

    if (prefix.find("(") != std::string::npos) {
        return;
    }

    for (auto &cmd : Commands) {
        if (std::equal(prefix.begin(), prefix.end(), cmd.begin())) {
            linenoiseAddCompletion(lc, cmd.c_str());
        }
    }

    std::vector<std::string> cmds =
        getModuleCompletions(programRef.get(), std::string(prefix));

    for (auto &cmd : cmds) {
        if (std::equal(prefix.begin(), prefix.end(), cmd.begin())) {
            linenoiseAddCompletion(lc, cmd.c_str());
        }
    }
}

const std::string HelpMessage = "\
Available commands:\n\
    <query> \n\
        where <query> has structure module.predicate(term,...)\n\
    import  </path/to/program.4ql>\n\
        import and evaluate 4ql program\n\
    help\n\
        print this\n\
    clear\n\
        clear modules from memory\n\
    modules\n\
        print a list of modules\n\
    exit\n\
        exit the program";

void InvalidSyntax() { std::cerr << "Invalid syntax." << std::endl; }

void printModules(std::vector<std::string> modules) {
    std::cout << "Modules:" << std::endl;
    for (auto &m : modules) {
        std::cout << std::string(4, ' ') << m << std::endl;
    }
}

bool inputEnd(int code) {
    typedef CmdLineScanner::Code Code;
    if (code != Code::End) {
        InvalidSyntax();
        return false;
    }
    return true;
}

void handleCliInput(Program &program, Lexer &lexer, int code) {
    typedef CmdLineScanner::Code Code;
    switch (code) {
        case Code::Help:
            std::cout << HelpMessage << std::endl;
            break;

        case Code::Clear:
            program.clear();
            std::cout << "Cleared modules." << std::endl;
            break;

        case Code::Modules:
            printModules(program.listModules());
            break;
        case Code::ImportQuoted:
        case Code::Import: {
            QlDeserializer qlD;
            std::string path = lexer.getToken();
            if (!inputEnd(lexer.scan())) {
                return;
            }

            if (code == Code::ImportQuoted) {
                path = path.substr(1, path.length() - 2);
            }

            qlD.parse(fileToString(path));
            program.addModules(qlD.getModules());
            program.evaluate();
            break;
        }

        case Code::Select: {
            std::string query = lexer.getToken();
            std::cout << query << std::endl;

            if (!inputEnd(lexer.scan())) {
                return;
            }

            std::cout << program.query(query) << std::endl;
            break;
        }
    }
}

void SyntaxError() {
    std::cerr << "Invalid syntax. Type 'help' to see list of commands."
              << std::endl;
}

void interactiveCli(Program &program) {

    linenoiseInstallWindowChangeHandler();

    namespace ph = std::placeholders;
    linenoiseSetCompletionCallback(
        std::bind(completion, ph::_1, ph::_2, std::ref(program)));

    char const *prompt = "\x1b[1;32mfovris\x1b[0m> ";

    Lexer lexer(std::make_unique<CmdLineScanner>());
    Lexer::TokenCode code;

    typedef CmdLineScanner::Code Code;

    while (true) {
        char *result = linenoise(prompt);

        if (result == NULL) {
            break;
        }

        std::string input(result);

        linenoiseHistoryAdd(result);

        free(result);

        lexer.reset(input.c_str(), input.length());

        code = lexer.scan();

        if (code == Code::Exit && inputEnd(lexer.scan())) {
            std::cout << "Good bye" << std::endl;
            break;
        }

        if (code == Lexer::ErrorCode || code == Code::String) {
            SyntaxError();
            continue;
        }

        handleCliInput(program, lexer, code);
    }

    linenoiseHistoryFree();
}

Algorithm getAlgorithm(std::string alg) {
    if (alg == "N") {
        return Algorithm::Naive;
    } else if (alg == "SN") {
        return Algorithm::Seminaive;
    }
    throw NoSuchAlgorithm(alg);
}
