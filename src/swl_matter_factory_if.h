#ifndef _SOLAR_WIND_LISP_MATTER_FACTORY_IF_H_
#define _SOLAR_WIND_LISP_MATTER_FACTORY_IF_H_

#include "swl_matter_if.h"
#include "swl_expr.h"
#include "swl_prim_proc.h"
#include "swl_proc.h"
#include "swl_future.h"
#include "swl_scoped_env.h"

namespace SolarWindLisp
{

class InterpreterIF;

class IMatterFactory
{
public:
    virtual ~IMatterFactory()
    {
    }

    virtual ExprPtr create_atom() = 0;
    virtual CompositeExprPtr create_molecule() = 0;
    virtual FuturePtr create_future(MatterPtr expr, ScopedEnvPtr env,
            InterpreterIF * interpreter) = 0;
    virtual ProcPtr create_proc(MatterPtr params, MatterPtr body,
            ScopedEnvPtr env) = 0;
    virtual ScopedEnvPtr create_env(ScopedEnvPtr ext = NULL) = 0;
};

class SimpleMatterFactory: public IMatterFactory
{
public:
    ExprPtr create_atom()
    {
        return Expr::create();
    }

    CompositeExprPtr create_molecule()
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

#endif // _SOLAR_WIND_LISP_MATTER_FACTORY_IF_H_
