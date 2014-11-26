#ifndef _SOLAR_WIND_LISTP_PRIM_PROC_H_
#define _SOLAR_WIND_LISTP_PRIM_PROC_H_

#include "swl_matter_if.h"
#include "pretty_message.h"

namespace SolarWindLisp
{

class IMatterFactory;

class IPrimProc: public IMatter
{
public:
    matter_type_t matter_type() const
    {
        return matter_prim_proc;
    }

    virtual bool run(const IMatter * ops, IMatter ** result, IMatterFactory * factory) = 0;
    virtual bool check_operands(const IMatter *ops) const = 0;
    virtual const char * name() const = 0;
};

class PrimProcAdd: public IPrimProc
{
public:
    bool run(const IMatter * ops, IMatter ** result, IMatterFactory * factory);
    bool check_operands(const IMatter * ops) const
    {
        // TODO
        return true;
    }

    const char * name() const
    {
        return "+";
    }

    std::string debug_string(bool compact = true, int level = 0,
            const char * indent_seq = DEFAULT_INDENT_SEQ) const
    {
        return "PrimProcAdd{}";
    }
};

#if 0
// arithmetic
static bool _prim_add(const CompositeExpr * operands, IMatter ** result);
static bool _prim_sub(const CompositeExpr * operands, IMatter ** result);
static bool _prim_mul(const CompositeExpr * operands, IMatter ** result);
static bool _prim_div(const CompositeExpr * operands, IMatter ** result);
static bool _prim_mod(const CompositeExpr * operands, IMatter ** result);
// relative
static bool _prim_eq(const IMatter * operands, IMatter &result);
static bool _prim_ne(const IMatter * operands, IMatter &result);
static bool _prim_lt(const IMatter * operands, IMatter &result);
static bool _prim_le(const IMatter * operands, IMatter &result);
static bool _prim_gt(const IMatter * operands, IMatter &result);
static bool _prim_ge(const IMatter * operands, IMatter &result);
// not
static bool _prim_not(const IMatter * operands, IMatter &result);
// bitwise ops
static bool _prim_shift(const IMatter * operands, IMatter &result);
static bool _prim_bitwise_and(const IMatter * operands, IMatter &result);
static bool _prim_bitwise_or(const IMatter * operands, IMatter &result);
static bool _prim_bitwise_xor(const IMatter * operands, IMatter &result);
static bool _prim_bitwise_not(const IMatter * operands, IMatter &result);
// sleep
static bool _prim_sleep(const IMatter * operands, IMatter &result);
// string ops
static bool _prim_strlen(const IMatter * operands, IMatter &result);
static bool _prim_format(const IMatter * operands, IMatter &result);
static bool _prim_print(const IMatter * operands, IMatter &result);
static bool _prim_println(const IMatter * operands, IMatter &result);
static bool _prim_printf(const IMatter * operands, IMatter &result);
#endif

} // namespace SolarWindLisp

#endif // _SOLAR_WIND_LISTP_PRIM_PROC_H_
