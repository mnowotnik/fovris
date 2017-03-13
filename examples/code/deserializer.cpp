#include "fovris/QlDeserializer.h"
#include <iostream>

int main() {
  fovris::QlDeserializer sp;
  try {
    sp.parse("module p:\
                        relations:\
                            parent(literal,literal).\
                            ancestor(literal,literal).\
                        rules:\
                            ancestor(X,Y):- ancestor(X,Z),parent(Z,Y).\
                            ancestor(X,Y):- parent(X,Y).\
                        facts:\
                            parent(alice,bob).\
                            parent(alice,bill).\
                            parent(bob,carol).\
                            parent(carol,dennis).\
                            parent(carol,david).\
                    end.");
  } catch (fovris::ParsingError &e) {
    std::cerr << "error:" << e.getLine() << ':' << e.getPos() << ": "
              << e.what() << std::endl;
    return 1;
  }
  std::vector<fovris::Module> modules = sp.getModules();
  std::vector<fovris::Query> queries = sp.getQueries();
}
