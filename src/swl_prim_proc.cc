#include "swl_expr.h"
#include "swl_prim_proc.h"
#include "swl_matter_factory_if.h"

namespace SolarWindLisp
{

bool PrimProcAdd::run(const IMatter * ops, IMatter ** result, IMatterFactory * factory)
{
    PRETTY_MESSAGE(stderr, "execute: `%s' ...", ops->debug_string(true).c_str());
    if (!ops->is_molecule()) {
        return false;
    }

    if (!result) {
        return true;
    }

    const CompositeExpr * ce = static_cast<const CompositeExpr *>(ops);
    if (!ce->rewind() || !ce->has_next()) {
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
            PRETTY_MESSAGE(stderr, "operand is not numberic: `%s'", e->debug_string().c_str());
            return false;
        }
    }

    if (result) {
        Expr * res = factory->create_atom();
        if (!res) {
            return false;
        }

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

} // namespace SolarWindLisp
