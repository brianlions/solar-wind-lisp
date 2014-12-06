/*
 * file name:           src/future.cc
 *
 * author:              Brian Yi ZHANG
 * email:               brianlions@gmail.com
 * date created:        Mon Nov 24 23:22:04 2014 CST
 */

#include "future.h"
#include "interpreter.h"

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
