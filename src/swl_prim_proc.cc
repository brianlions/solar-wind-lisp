/*
 * file name:           src/swl_prim_proc.cc
 *
 * author:              Brian Yi ZHANG
 * email:               brianlions@gmail.com
 * date created:        Wed Nov 26 16:41:51 2014 CST
 */

#include "swl_expr.h"
#include "swl_prim_proc.h"
#include "swl_matter_factory_if.h"

namespace SolarWindLisp
{

bool PrimProcAdd::run(const MatterPtr &ops, MatterPtr &result,
        IMatterFactory * factory)
{
    const CompositeExpr * ce = static_cast<const CompositeExpr *>(ops.get());
    if (!ce->rewind()) {
        return false;
    }

    ExprPtr res = factory->create_atom();
    if (!res.get()) {
        return false;
    }

    int64_t result_i64 = 0;
    long double result_ld = 0;
    int64_t temp_i64 = 0;
    long double temp_ld = 0;
    bool integer = true;
    while (ce->has_next()) {
        const Expr * e = static_cast<const Expr *>(ce->get_next().get());
        if (e->is_integer()) {
            e->to_i64(temp_i64);
            if (integer) {
                result_i64 += temp_i64;
            }
            else {
                result_ld += temp_i64;
            }
        }
        else if (e->is_real()) {
            e->to_long_double(temp_ld);
            if (integer) {
                integer = false;
                result_ld = static_cast<long double>(result_i64);
            }
            result_ld += temp_ld;
        }
        else {
            PRETTY_MESSAGE(stderr, "operand is not numberic: `%s'",
                    e->debug_string().c_str());
            return false;
        }
    }

    if (integer) {
        res->set_i64(result_i64);
    }
    else {
        res->set_long_double(result_ld);
    }
    result = res;

    return true;
}

bool PrimProcSub::run(const MatterPtr &ops, MatterPtr &result,
        IMatterFactory * factory)
{
    const CompositeExpr * ce = static_cast<const CompositeExpr *>(ops.get());
    if (!ce->rewind()) {
        return false;
    }

    ExprPtr res = factory->create_atom();
    if (!res.get()) {
        return false;
    }

    int64_t result_i64 = 0;
    long double result_ld = 0;
    int64_t temp_i64 = 0;
    long double temp_ld = 0;
    bool integer = true;
    const Expr * first = static_cast<const Expr *>(ce->get_next().get());
    if (first->is_integer()) {
        first->to_i64(result_i64);
    }
    else if (first->is_real()) {
        integer = false;
        first->to_long_double(result_ld);
    }
    else {
        return false;
    }

    if (ce->size() == 1) {
        if (integer) {
            result_i64 = - result_i64;
        }
        else {
            result_ld = - result_ld;
        }
    }
    else {
        while (ce->has_next()) {
            const Expr * e = static_cast<const Expr *>(ce->get_next().get());
            if (e->is_integer()) {
                e->to_i64(temp_i64);
                if (integer) {
                    result_i64 -= temp_i64;
                }
                else {
                    result_ld -= temp_i64;
                }
            }
            else if (e->is_real()) {
                e->to_long_double(temp_ld);
                if (integer) {
                    integer = false;
                    result_ld = static_cast<long double>(result_i64);
                }
                result_ld -= temp_ld;
            }
            else {
                return false;
            }
        }
    }

    if (integer) {
        res->set_i64(result_i64);
    }
    else {
        res->set_long_double(result_ld);
    }
    result = res;

    return true;
}

bool PrimProcMul::run(const MatterPtr &ops, MatterPtr &result,
        IMatterFactory * factory)
{
    const CompositeExpr * ce = static_cast<const CompositeExpr *>(ops.get());
    if (!ce->rewind()) {
        return false;
    }

    ExprPtr res = factory->create_atom();
    if (!res.get()) {
        return false;
    }

    int64_t result_i64 = 1;
    long double result_ld = 1;
    int64_t temp_i64 = 0;
    long double temp_ld = 0;
    bool integer = true;
    while (ce->has_next()) {
        const Expr * e = static_cast<const Expr *>(ce->get_next().get());
        if (e->is_integer()) {
            e->to_i64(temp_i64);
            if (integer) {
                result_i64 *= temp_i64;
            }
            else {
                result_ld *= temp_i64;
            }
        }
        else if (e->is_real()) {
            e->to_long_double(temp_ld);
            if (integer) {
                integer = false;
                result_ld = static_cast<long double>(result_i64);
            }
            result_ld *= temp_ld;
        }
        else {
            PRETTY_MESSAGE(stderr, "operand is not numberic: `%s'",
                    e->debug_string().c_str());
            return false;
        }
    }

    if (integer) {
        res->set_i64(result_i64);
    }
    else {
        res->set_long_double(result_ld);
    }
    result = res;

    return true;
}

bool PrimProcDiv::run(const MatterPtr &ops, MatterPtr &result,
        IMatterFactory * factory)
{
    const CompositeExpr * ce = static_cast<const CompositeExpr *>(ops.get());
    if (!ce->rewind()) {
        return false;
    }

    ExprPtr res = factory->create_atom();
    if (!res.get()) {
        return false;
    }

    long double result_ld = 0;
    long double temp_ld = 0;

    const Expr * first = static_cast<const Expr *>(ce->get_next().get());
    first->to_long_double(result_ld);
    if (ce->size() == 1) {
        if (result_ld == 0) {
            return false;
        }
        result_ld = 1.0 / result_ld;
    }
    else {
        while (ce->has_next()) {
            const Expr * e = static_cast<const Expr *>(ce->get_next().get());
            if (!e->to_long_double(temp_ld) || temp_ld == 0) {
                return false;
            }
            result_ld /= temp_ld;
        }
    }

    res->set_long_double(result_ld);
    result = res;

    return true;
}

#define PRIM_PROC_RUN_IMPL_MACRO_BEGIN(name)                            \
bool PrimProc##name::run(const MatterPtr &ops, MatterPtr &result,       \
        IMatterFactory * factory)                                       \
{                                                                       \
    const CompositeExpr * ce =                                          \
            static_cast<const CompositeExpr*>(ops.get());               \
    if (!ce->rewind()) {                                                \
        return false;                                                   \
    }                                                                   \
    ExprPtr res = factory->create_atom();                               \
    if (!res.get()) {                                                   \
        return false;                                                   \
    }                                                                   \
    long double first = 0;                                              \
    long double second = 0;                                             \
    const Expr * e = NULL;                                              \
    e = static_cast<const Expr *>(ce->get(0).get());                    \
    e->to_long_double(first);                                           \
    e = static_cast<const Expr *>(ce->get(1).get());                    \
    e->to_long_double(second);                                          \
    res->set_bool(first 

#define PRIM_PROC_RUN_IMPL_MACRO_END()                                  \
            second);                                                    \
    result = res;                                                       \
    return true;                                                        \
}
PRIM_PROC_RUN_IMPL_MACRO_BEGIN(Eq) == PRIM_PROC_RUN_IMPL_MACRO_END()
PRIM_PROC_RUN_IMPL_MACRO_BEGIN(Ne) != PRIM_PROC_RUN_IMPL_MACRO_END()
PRIM_PROC_RUN_IMPL_MACRO_BEGIN(Lt) <  PRIM_PROC_RUN_IMPL_MACRO_END()
PRIM_PROC_RUN_IMPL_MACRO_BEGIN(Le) <= PRIM_PROC_RUN_IMPL_MACRO_END()
PRIM_PROC_RUN_IMPL_MACRO_BEGIN(Gt) >  PRIM_PROC_RUN_IMPL_MACRO_END()
PRIM_PROC_RUN_IMPL_MACRO_BEGIN(Ge) >= PRIM_PROC_RUN_IMPL_MACRO_END()
#undef PRIM_PROC_RUN_IMPL_MACRO_BEGIN
#undef PRIM_PROC_RUN_IMPL_MACRO_END

} // namespace SolarWindLisp
