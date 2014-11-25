#ifndef _SOLAR_WIND_LISP_FUTURE_H_
#define _SOLAR_WIND_LISP_FUTURE_H_

#include <new>
#include "swl_expr_if.h"
#include "pretty_message.h"

namespace SolarWindLisp
{

class InterpreterIF;

class Future: public IExpr
{
public:
    matter_type_t matter_type() const
    {
        return matter_future;
    }

    static Future * create(IMatter * expr, ScopedEnv * env,
            InterpreterIF * interpreter)
    {
        return new (std::nothrow) Future(expr, env, interpreter);
    }

    bool value(IMatter ** result);

    std::string debug_string(bool compact = true, int level = 0,
            const char * indent_seq = DEFAULT_INDENT_SEQ) const
    {
        PRETTY_MESSAGE(stderr, "not a useful implementation");
        return "from Future::debug_string()";
    }

private:
    Future(IMatter * expr, ScopedEnv * env, InterpreterIF * interpreter) :
            _expr(expr), _env(env), _interpreter(interpreter), _value(NULL)
    {
    }

    IMatter * _expr;
    ScopedEnv * _env;
    InterpreterIF * _interpreter;
    IMatter * _value;
};

} // namespace SolarWindLisp

#endif // _SOLAR_WIND_LISP_FUTURE_H_
