#include "intent/interpreter/LineTagger.hpp"

namespace intent {

bool isMarkedLine(const ScriptLine& line)
{
    return isLine<ACTION>(line) ||
           isLine<CLOSE_SCENARIO>(line) ||
           isLine<START_SCENARIO>(line) ||
           isLine<SAYING>(line) ||
           isLine<STATE>(line) ||
           isLine<PLACE_HOLDER>(line) ||
           isLine<FALLBACK>(line);
}

}
