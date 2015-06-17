/*
 * file name:           include/prim_proc.h
 *
 * author:              Brian Yi ZHANG
 * email:               brianlions@gmail.com
 * date created:        Sat Nov 22 23:15:18 2014 CST
 */

#ifndef _SOLAR_WIND_LISP_PRIM_PROC_H_
#define _SOLAR_WIND_LISP_PRIM_PROC_H_

#include "matter.h"
#include "expr.h"
#include "pretty_message.h"

namespace SolarWindLisp
{

class MatterFactoryIF;

class PrimProcIF: public MatterIF
{
public:
    matter_type_t matter_type() const
    {
        return matter_prim_proc;
    }

    virtual bool run(const MatterPtr &ops, MatterPtr &result,
            MatterFactoryIF * factory) = 0;
    virtual bool check_operands(const MatterPtr &ops) const = 0;
    virtual const char * name() const = 0;
};

/*
 * Arithmetic Operations:
 *   + - * /
 *
 * Notes:
 *   Every operator requires AT LEAST ONE operand.
 *     (+ 5) => 5
 *     (- 5) => -5
 *     (* 5) => 5
 *     (/ 5) => 0.2
 *
 * TODO:
 *   1. too aggressive to use int64_t or long double when saving result
 *   2. does not handle overflow or underflow
 */
#define PROC_DECLARATION_ONE_PLUS_MACRO(cls_name, cls_repr, cls_repr2)  \
class PrimProc##cls_name: public PrimProcIF                             \
{                                                                       \
public:                                                                 \
    bool run(const MatterPtr &ops, MatterPtr &result,                   \
            MatterFactoryIF * factory);                                 \
                                                                        \
    bool check_operands(const MatterPtr &ops) const                     \
    {                                                                   \
        return ops->is_composite_expr() &&                              \
            static_cast<const CompositeExpr *>(ops.get())->size();      \
    }                                                                   \
                                                                        \
    const char * name() const                                           \
    {                                                                   \
        return cls_repr;                                                \
    }                                                                   \
                                                                        \
    std::string debug_string(bool compact = true, int level = 0,        \
            const char * indent_seq = DEFAULT_INDENT_SEQ) const         \
    {                                                                   \
        return cls_repr2;                                               \
    }                                                                   \
                                                                        \
    std::string to_string() const                                       \
    {                                                                   \
        return "instance of PrimProcIF";                                \
    }                                                                   \
                                                                        \
    static PrimProcPtr create()                                         \
    {                                                                   \
        return PrimProcPtr(new (std::nothrow) PrimProc##cls_name());    \
    }                                                                   \
};
PROC_DECLARATION_ONE_PLUS_MACRO(Add, "+", "PrimProcAdd{}")
PROC_DECLARATION_ONE_PLUS_MACRO(Sub, "-", "PrimProcSub{}")
PROC_DECLARATION_ONE_PLUS_MACRO(Mul, "*", "PrimProcMul{}")
PROC_DECLARATION_ONE_PLUS_MACRO(Div, "/", "PrimProcDiv{}")
#undef PROC_DECLARATION_ONE_PLUS_MACRO

/*
 * Operations:
 *   modulo (mod) and relative operators (= != < <= > >=)
 *
 * Notes:
 *   Every operator requires EXACTLY TWO operands.
 *
 * TODO:
 *   mod is not implemented
 */
#define PROC_DECLARATION_EXACT_TWO_MACRO(cls_name, cls_repr, cls_repr2) \
class PrimProc##cls_name: public PrimProcIF                             \
{                                                                       \
public:                                                                 \
    bool run(const MatterPtr &ops, MatterPtr &result,                   \
            MatterFactoryIF * factory);                                 \
                                                                        \
    bool check_operands(const MatterPtr &ops) const                     \
    {                                                                   \
        return ops->is_composite_expr() &&                              \
            static_cast<const CompositeExpr *>(ops.get())->size() == 2; \
    }                                                                   \
                                                                        \
    const char * name() const                                           \
    {                                                                   \
        return cls_repr;                                                \
    }                                                                   \
                                                                        \
    std::string debug_string(bool compact = true, int level = 0,        \
            const char * indent_seq = DEFAULT_INDENT_SEQ) const         \
    {                                                                   \
        return cls_repr2;                                               \
    }                                                                   \
                                                                        \
    std::string to_string() const                                       \
    {                                                                   \
        return "instance of PrimProcIF";                                \
    }                                                                   \
                                                                        \
    static PrimProcPtr create()                                         \
    {                                                                   \
        return PrimProcPtr(new (std::nothrow) PrimProc##cls_name());    \
    }                                                                   \
};
PROC_DECLARATION_EXACT_TWO_MACRO(Mod, "mod",  "PrimProcMod{}")
PROC_DECLARATION_EXACT_TWO_MACRO(Eq,  "=",    "PrimProcEq{}")
PROC_DECLARATION_EXACT_TWO_MACRO(Ne,  "!=",   "PrimProcNe{}")
PROC_DECLARATION_EXACT_TWO_MACRO(Lt,  "<",    "PrimProcLt{}")
PROC_DECLARATION_EXACT_TWO_MACRO(Le,  "<=",   "PrimProcLe{}")
PROC_DECLARATION_EXACT_TWO_MACRO(Gt,  ">",    "PrimProcGt{}")
PROC_DECLARATION_EXACT_TWO_MACRO(Ge,  ">=",   "PrimProcGe{}")
#undef PROC_DECLARATION_EXACT_TWO_MACRO

#if 0
// not
static bool _prim_not(const MatterIF * operands, MatterIF &result);
// bitwise ops
static bool _prim_shift(const MatterIF * operands, MatterIF &result);
static bool _prim_bitwise_and(const MatterIF * operands, MatterIF &result);
static bool _prim_bitwise_or(const MatterIF * operands, MatterIF &result);
static bool _prim_bitwise_xor(const MatterIF * operands, MatterIF &result);
static bool _prim_bitwise_not(const MatterIF * operands, MatterIF &result);
// sleep
static bool _prim_sleep(const MatterIF * operands, MatterIF &result);
// string ops
static bool _prim_strlen(const MatterIF * operands, MatterIF &result);
static bool _prim_format(const MatterIF * operands, MatterIF &result);
static bool _prim_print(const MatterIF * operands, MatterIF &result);
static bool _prim_println(const MatterIF * operands, MatterIF &result);
static bool _prim_printf(const MatterIF * operands, MatterIF &result);
#endif

} // namespace SolarWindLisp

#endif // _SOLAR_WIND_LISP_PRIM_PROC_H_
