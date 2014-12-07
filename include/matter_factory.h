/*
 * file name:           include/matter_factory.h
 *
 * author:              Brian Yi ZHANG
 * email:               brianlions@gmail.com
 * date created:        Sat Nov 22 23:15:18 2014 CST
 */

#ifndef _SOLAR_WIND_LISP_MATTER_FACTORY_H_
#define _SOLAR_WIND_LISP_MATTER_FACTORY_H_

#include "matter.h"
#include "expr.h"
#include "prim_proc.h"
#include "proc.h"
#include "future.h"
#include "scoped_env.h"

namespace SolarWindLisp
{

class InterpreterIF;

class MatterFactoryIF
{
public:
    virtual ~MatterFactoryIF()
    {
    }

    virtual AtomPtr create_atom() = 0;
    virtual CompositeExprPtr create_composite_expr() = 0;
    virtual FuturePtr create_future(MatterPtr expr, ScopedEnvPtr env,
            InterpreterIF * interpreter) = 0;
    virtual ProcPtr create_proc(MatterPtr params, MatterPtr body,
            ScopedEnvPtr env) = 0;
    virtual ScopedEnvPtr create_env(ScopedEnvPtr ext = NULL) = 0;
};

class SimpleMatterFactory: public MatterFactoryIF
{
public:
    AtomPtr create_atom()
    {
        return Atom::create();
    }

    CompositeExprPtr create_composite_expr()
    {
        return CompositeExpr::create();
    }

    ProcPtr create_proc(MatterPtr params , MatterPtr body, ScopedEnvPtr env)
    {
        return Proc::create(params, body, env);
    }

    FuturePtr create_future(MatterPtr expr, ScopedEnvPtr env,
            InterpreterIF * interpreter)
    {
        return Future::create(expr, env, interpreter);
    }

    ScopedEnvPtr create_env(ScopedEnvPtr ext = NULL)
    {
        return ScopedEnv::create(ext);
    }
};

} // namespace SolarWindLisp

#endif // _SOLAR_WIND_LISP_MATTER_FACTORY_H_
