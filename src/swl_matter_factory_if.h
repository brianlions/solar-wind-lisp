#ifndef _SOLAR_WIND_LISP_MATTER_FACTORY_IF_H_
#define _SOLAR_WIND_LISP_MATTER_FACTORY_IF_H_

#include "swl_matter_if.h"
#include "swl_expr.h"
#include "swl_prim_proc.h"
#include "swl_proc.h"
#include "swl_future.h"

namespace SolarWindLisp
{

class IMatterFactory
{
public:
    virtual ~IMatterFactory()
    {
    }

    virtual Expr * create_atom() = 0;
    virtual CompositeExpr * create_molecule() = 0;
    virtual PrimProc * create_prim_proc() = 0;
    virtual Proc * create_proc() = 0;
    virtual Future * create_future() = 0;
    IMatter * create(IMatter::matter_type_t t);
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

    // TODO
    PrimProc * create_prim_proc()
    {
        return NULL;
    }

    // TODO
    Proc * create_proc()
    {
        return NULL;
    }

    // TODO
    Future * create_future()
    {
        return NULL;
    }
};

} // namespace SolarWindLisp

#endif // _SOLAR_WIND_LISP_MATTER_FACTORY_IF_H_
