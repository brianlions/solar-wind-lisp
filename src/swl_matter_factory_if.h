#ifndef _SOLAR_WIND_LISP_MATTER_FACTORY_IF_H_
#define _SOLAR_WIND_LISP_MATTER_FACTORY_IF_H_

#include "swl_matter_if.h"
#include "swl_expr.h"
#include "swl_prim_proc.h"
#include "swl_proc.h"
#include "swl_future.h"

namespace SolarWindLisp
{

class InterpreterIF;

class IMatterFactory
{
public:
    virtual ~IMatterFactory()
    {
    }

    virtual Expr * create_atom() = 0;
    virtual CompositeExpr * create_molecule() = 0;
    virtual Future * create_future(IMatter * expr, ScopedEnv * env,
            InterpreterIF * interpreter) = 0;
    virtual Proc * create_proc(IMatter * params, IMatter * body,
            ScopedEnv * env) = 0;
};

class SimpleMatterFactory: public IMatterFactory
{
public:
    Expr * create_atom()
    {
        return Expr::create();
    }

    CompositeExpr * create_molecule()
    {
        return CompositeExpr::create();
    }

    Proc * create_proc(IMatter * params , IMatter * body, ScopedEnv * env)
    {
        return Proc::create(params, body, env);
    }

    Future * create_future(IMatter * expr, ScopedEnv * env,
            InterpreterIF * interpreter);
};

} // namespace SolarWindLisp

#endif // _SOLAR_WIND_LISP_MATTER_FACTORY_IF_H_
