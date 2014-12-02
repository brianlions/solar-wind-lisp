#ifndef _SOLAR_WIND_LISP_FUTURE_H_
#define _SOLAR_WIND_LISP_FUTURE_H_

#include <new>
#include "swl_matter_if.h"
#include "pretty_message.h"

namespace SolarWindLisp
{

class InterpreterIF;

class Future: public IMatter
{
public:
    matter_type_t matter_type() const
    {
        return matter_future;
    }

    static FuturePtr create(MatterPtr expr, ScopedEnvPtr env,
            InterpreterIF * interpreter)
    {
        return FuturePtr(new (std::nothrow) Future(expr, env, interpreter));
    }

    bool value(MatterPtr &result);

    std::string debug_string(bool compact = true, int level = 0,
            const char * indent_seq = DEFAULT_INDENT_SEQ) const
    {
        return "Future::debug_string()";
    }

private:
    Future(MatterPtr expr, ScopedEnvPtr env, InterpreterIF * interpreter) :
            _expr(expr), _env(env), _interpreter(interpreter), _value(NULL)
    {
    }

    MatterPtr _expr;
    ScopedEnvPtr _env;
    InterpreterIF * _interpreter;
    MatterPtr _value;
};

} // namespace SolarWindLisp

#endif // _SOLAR_WIND_LISP_FUTURE_H_
