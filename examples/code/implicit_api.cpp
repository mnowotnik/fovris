#include "Module.h"
#include "Program.h"
#include <iostream>

int main() {
    using namespace fovris;
    Module m("m", 
            {
                {"parent", {TermType::Id, TermType::Id}},
                {"ancestor", {TermType::Id, TermType::Id}}
            },
            {
                {{"ancestor", {Var{"X"}, Var{"Y"}}}, {{"ancestor", {Var{"X"}, Var{"Z"}}}, {"parent", {Var{"Z"}, Var{"Y"}}}}},
                {{"ancestor", {Var{"X"}, Var{"Y"}}}, {{"parent", {Var{"X"}, Var{"Y"}}}}}
            },
            {
                {"parent", {"alice", "bob"}},
                {"parent", {"alice", "bill"}},
                {"parent", {"bob", "carol"}},
                {"parent", {"carol", "dennis"}},
                {"parent", {"carol", "david"}}
            });
    Program program(Algorithm::Naive);
    program.addModule(m);
    program.evaluate();
    auto results = program.query("m","ancestor",{Var{"X"},Var{"Y"}});
    std::cout << "Results:" << std::endl;
    for(auto& result: results){
        std::cout << result << std::endl;
    }
}
