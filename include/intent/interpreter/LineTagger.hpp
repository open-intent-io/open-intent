#ifndef INTENT_INTERPRETER_LINETAGGER_HPP
#define INTENT_INTERPRETER_LINETAGGER_HPP

#include "intent/interpreter/Interpreter.hpp"

namespace intent {

enum MARKER {
  ACTION = '#',
  SAYING = '-',
  STATE = '@',
  START_SCENARIO = '{',
  CLOSE_SCENARIO = '}',
  PLACE_HOLDER = '_',
  FALLBACK = '*'
};

template <MARKER marker>
bool isLine(const ScriptLine& line) {
  return line.content[0] == marker;
}

bool isMarkedLine(const ScriptLine& line);

}

#endif
