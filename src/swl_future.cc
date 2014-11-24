#include "swl_future.h"

namespace SolarWindLisp
{
bool Future::value(IMatter ** result)
{
    if (!_value) {
        if (!InterpreterIF::_force_eval(_expr, _env, _interpreter, &_value)) {
            return false;
        }
    }

    *result = _value;
    return true;
}

} // namespace SolarWindLisp
