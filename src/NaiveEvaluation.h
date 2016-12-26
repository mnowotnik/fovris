#ifndef DEFAULTNAIVEEVALUATION_H_5C7GNRKX
#define DEFAULTNAIVEEVALUATION_H_5C7GNRKX
#include "Evaluation.h"
namespace fovris {


class NaiveEvaluation : public Evaluation {
  public:
    void evaluateKB(KnowledgeBase &kb);
};
} // fovris
#endif /* end of include guard: DEFAULTNAIVEEVALUATION_H_5C7GNRKX */
