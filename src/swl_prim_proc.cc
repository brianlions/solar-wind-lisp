#include "swl_expr.h"
#include "swl_prim_proc.h"
#include "swl_matter_factory_if.h"

namespace SolarWindLisp
{

bool PrimProcAdd::run(const IMatter * ops, IMatter ** result,
        IMatterFactory * factory)
{
    const CompositeExpr * ce = static_cast<const CompositeExpr *>(ops);
    if (!ce->rewind()) {
        return false;
    }

    Expr * res = factory->create_atom();
    if (!res) {
        return false;
    }

    int64_t result_i64 = 0;
    long double result_ld = 0;
    int64_t temp_i64 = 0;
    long double temp_ld = 0;
    bool integer = true;
    while (ce->has_next()) {
        const Expr * e = static_cast<const Expr *>(ce->get_next());
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

    if (result) {
        if (integer) {
            res->set_i64(result_i64);
        }
        else {
            res->set_long_double(result_ld);
        }
        *result = res;
    }

    return true;
}

bool PrimProcSub::run(const IMatter * ops, IMatter ** result,
        IMatterFactory * factory)
{
    const CompositeExpr * ce = static_cast<const CompositeExpr *>(ops);
    if (!ce->rewind()) {
        return false;
    }

    Expr * res = factory->create_atom();
    if (!res) {
        return false;
    }

    int64_t result_i64 = 0;
    long double result_ld = 0;
    int64_t temp_i64 = 0;
    long double temp_ld = 0;
    bool integer = true;
    const Expr * first = static_cast<const Expr *>(ce->get_next());
    if (first->is_integer()) {
        first->to_i64(result_i64);
    }
    else if (first->is_real()) {
        integer = false;
        first->to_long_double(result_ld);
    }
    else {
        // FIXME delete res or else mem leak
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
            const Expr * e = static_cast<const Expr *>(ce->get_next());
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
                // FIXME delete res or else mem leak
                return false;
            }
        }
    }

    if (result) {
        if (integer) {
            res->set_i64(result_i64);
        }
        else {
            res->set_long_double(result_ld);
        }
        *result = res;
    }

    return true;
#if 0
    int64_t result_i64 = 0;
    long double result_ld = 0;
    int64_t temp_i64 = 0;
    long double temp_ld = 0;
    bool integer = true;
    while (ce->has_next()) {
        const Expr * e = static_cast<const Expr *>(ce->get_next());
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
            PRETTY_MESSAGE(stderr, "operand is not numberic: `%s'",
                    e->debug_string().c_str());
            return false;
        }
    }

    if (result) {
        if (integer) {
            res->set_i64(result_i64);
        }
        else {
            res->set_long_double(result_ld);
        }
        *result = res;
    }

    return true;
#endif
}

bool PrimProcMul::run(const IMatter * ops, IMatter ** result,
        IMatterFactory * factory)
{
    const CompositeExpr * ce = static_cast<const CompositeExpr *>(ops);
    if (!ce->rewind()) {
        return false;
    }

    Expr * res = factory->create_atom();
    if (!res) {
        return false;
    }

    int64_t result_i64 = 1;
    long double result_ld = 1;
    int64_t temp_i64 = 0;
    long double temp_ld = 0;
    bool integer = true;
    while (ce->has_next()) {
        const Expr * e = static_cast<const Expr *>(ce->get_next());
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

    if (result) {
        if (integer) {
            res->set_i64(result_i64);
        }
        else {
            res->set_long_double(result_ld);
        }
        *result = res;
    }

    return true;
}

bool PrimProcDiv::run(const IMatter * ops, IMatter ** result,
        IMatterFactory * factory)
{
    const CompositeExpr * ce = static_cast<const CompositeExpr *>(ops);
    if (!ce->rewind()) {
        return false;
    }

    Expr * res = factory->create_atom();
    if (!res) {
        return false;
    }

    long double result_ld = 0;
    long double temp_ld = 0;

    const Expr * first = static_cast<const Expr *>(ce->get_next());
    first->to_long_double(result_ld);
    if (ce->size() == 1) {
        if (result_ld == 0) {
            // FIXME delete res or else mem leak
            return false;
        }
        result_ld = 1.0 / result_ld;
    }
    else {
        while (ce->has_next()) {
            const Expr * e = static_cast<const Expr *>(ce->get_next());
            if (!e->to_long_double(temp_ld) || temp_ld == 0) {
                // FIXME delete res or else mem leak
                return false;
            }
            result_ld /= temp_ld;
        }
    }

    res->set_long_double(result_ld);
    if (result) {
        *result = res;
    }

    return true;
}

} // namespace SolarWindLisp
