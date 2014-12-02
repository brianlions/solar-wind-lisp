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

    virtual bool run(const MatterPtr &ops, MatterPtr &result,
            IMatterFactory * factory) = 0;
    virtual bool check_operands(const MatterPtr &ops) const = 0;
    virtual const char * name() const = 0;
};

class PrimProcAdd: public IPrimProc
{
public:
    bool run(const MatterPtr &ops, MatterPtr &result, IMatterFactory * factory);

    bool check_operands(const MatterPtr &ops) const
    {
        return ops->is_molecule()
            && static_cast<const CompositeExpr *>(ops.get())->size();
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

    static PrimProcPtr create()
    {
        return PrimProcPtr(new (std::nothrow) PrimProcAdd());
    }
};

class PrimProcSub: public IPrimProc
{
public:
    bool run(const MatterPtr &ops, MatterPtr &result, IMatterFactory * factory);

    bool check_operands(const MatterPtr &ops) const
    {
        return ops->is_molecule()
            && static_cast<const CompositeExpr *>(ops.get())->size();
    }

    const char * name() const
    {
        return "-";
    }

    std::string debug_string(bool compact = true, int level = 0,
            const char * indent_seq = DEFAULT_INDENT_SEQ) const
    {
        return "PrimProcSub{}";
    }

    static PrimProcPtr create()
    {
        return PrimProcPtr(new (std::nothrow) PrimProcSub());
    }
};

class PrimProcMul: public IPrimProc
{
public:
    bool run(const MatterPtr &ops, MatterPtr &result, IMatterFactory * factory);

    bool check_operands(const MatterPtr &ops) const
    {
        return ops->is_molecule()
            && static_cast<const CompositeExpr *>(ops.get())->size();
    }

    const char * name() const
    {
        return "*";
    }

    std::string debug_string(bool compact = true, int level = 0,
            const char * indent_seq = DEFAULT_INDENT_SEQ) const
    {
        return "PrimProcMul{}";
    }

    static PrimProcPtr create()
    {
        return PrimProcPtr(new (std::nothrow) PrimProcMul());
    }
};

class PrimProcDiv: public IPrimProc
{
public:
    bool run(const MatterPtr &ops, MatterPtr &result, IMatterFactory * factory);

    bool check_operands(const MatterPtr &ops) const
    {
        return ops->is_molecule()
            && static_cast<const CompositeExpr *>(ops.get())->size();
    }

    const char * name() const
    {
        return "/";
    }

    std::string debug_string(bool compact = true, int level = 0,
            const char * indent_seq = DEFAULT_INDENT_SEQ) const
    {
        return "PrimProcDiv{}";
    }

    static PrimProcPtr create()
    {
        return PrimProcPtr(new (std::nothrow) PrimProcDiv());
    }
};

class PrimProcMod: public IPrimProc
{
public:
    bool run(const MatterPtr &ops, MatterPtr &result, IMatterFactory * factory);

    bool check_operands(const MatterPtr &ops) const
    {
        return ops->is_molecule()
                && static_cast<const CompositeExpr *>(ops.get())->size() == 2;
    }

    const char * name() const
    {
        return "mod";
    }

    std::string debug_string(bool compact = true, int level = 0,
            const char * indent_seq = DEFAULT_INDENT_SEQ) const
    {
        return "PrimProcMod{}";
    }

    static PrimProcPtr create()
    {
        return PrimProcPtr(new (std::nothrow) PrimProcMod());
    }
};

#if 0
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
