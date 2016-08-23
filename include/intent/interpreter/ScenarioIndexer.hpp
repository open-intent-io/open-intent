#ifndef INTENT_INTERPRETER_SCENARIOINDEXER_HPP
#define INTENT_INTERPRETER_SCENARIOINDEXER_HPP

#include "intent/interpreter/Interpreter.hpp"

namespace intent {

void indexScenario(const Scenario& scenario,
                   InquiryToReplies& inquiryToReplies);

void extractScenarios(const std::string& script,
                      Scenarios& scenarios);

}

#endif

