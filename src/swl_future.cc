#include "swl_future.h"
#include "swl_interpreter.h"

namespace SolarWindLisp
{

bool Future::value(MatterPtr &result)
{
    if (!_value.get()) {
        if (!InterpreterIF::_force_eval(_expr, _env, _interpreter, _value)) {
            return false;
        }
    }

    result = _value;
    return true;
}

} // namespace SolarWindLisp
