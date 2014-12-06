/*
 * file name:           include/future.h
 *
 * author:              Brian Yi ZHANG
 * email:               brianlions@gmail.com
 * date created:        Sat Nov 22 23:15:18 2014 CST
 */

#ifndef _SOLAR_WIND_LISP_FUTURE_H_
#define _SOLAR_WIND_LISP_FUTURE_H_

#include <new>
#include "matter.h"
#include "pretty_message.h"

namespace SolarWindLisp
{

class InterpreterIF;

class Future: public MatterIF
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

    std::string to_string() const
    {
        return "instance of Future";
    }

    virtual ~Future()
    {
        _env.reset();
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
