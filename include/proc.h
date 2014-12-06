/*
 * file name:           include/proc.h
 *
 * author:              Brian Yi ZHANG
 * email:               brianlions@gmail.com
 * date created:        Sat Nov 22 23:15:18 2014 CST
 */

#ifndef _SOLAR_WIND_LISP_PROC_H_
#define _SOLAR_WIND_LISP_PROC_H_

#include "matter.h"
#include "proc.h"
#include "scoped_env.h"
#include "pretty_message.h"

namespace SolarWindLisp
{

class Proc: public MatterIF
{
public:
    matter_type_t matter_type() const
    {
        return matter_proc;
    }

    static ProcPtr create(MatterPtr params, MatterPtr body, ScopedEnvPtr env)
    {
        return ProcPtr(new (std::nothrow) Proc(params, body, env));
    }

    bool get_params(MatterPtr &result)
    {
        result = _params;
        return true;
    }

    bool get_body(MatterPtr &result)
    {
        result = _body;
        return true;
    }

    bool get_env(ScopedEnvPtr &result)
    {
        result = _env;
        return true;
    }

    std::string debug_string(bool compact = true, int level = 0,
            const char * indent_seq = DEFAULT_INDENT_SEQ) const
    {
        return "Proc::debug_string()";
    }

    std::string to_string() const
    {
        return "instance of Proc";
    }

    virtual ~Proc()
    {
        _env.reset();
    }

private:
    Proc(MatterPtr params, MatterPtr body, ScopedEnvPtr env) :
            _params(params), _body(body), _env(env)
    {
    }

    MatterPtr _params;
    MatterPtr _body;
    ScopedEnvPtr _env;
};

} // namespace SolarWindLisp

#endif // _SOLAR_WIND_LISP_PROC_H_
