#ifndef _SOLAR_WIND_LISP_COMPOSITE_H_
#define _SOLAR_WIND_LISP_COMPOSITE_H_

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <errno.h>

#include <limits>
#include <string>

namespace SolarWindLisp
{

class Expr;
class CompositeExpr;

class IExpr
{
    friend class Expr;
    friend class CompositeExpr;

public:
    enum expr_type
    {
        type_none = 0,
        type_bool,
        type_i32,
        type_u32,
        type_i64,
        type_u64,
        type_float,
        type_double,
        type_long_double,
        type_string,
        type_composite,
        // add type names before this one!
        num_of_supporte_types,
    };

    IExpr() :
            _type(type_none), _prev(NULL), _next(NULL)
    {
    }

    virtual ~IExpr()
    {
    }

#define declare_type_test_macro(name)   virtual bool is_##name() const = 0
    declare_type_test_macro(none);
    declare_type_test_macro(bool);
    declare_type_test_macro(i32);
    declare_type_test_macro(u32);
    declare_type_test_macro(i64);
    declare_type_test_macro(u64);
    declare_type_test_macro(float);
    declare_type_test_macro(double);
    declare_type_test_macro(long_double);
    declare_type_test_macro(string);
    declare_type_test_macro(composite);
#undef declare_type_test_macro

    bool is_integer() const
    {
        return _type != type_composite //
        && _type != type_none //
        && (_type == type_bool //
        || _type == type_i32 //
        || _type == type_u32 //
        || _type == type_i64 //
        || _type == type_u64);
    }

    bool is_real() const
    {
        return _type != type_composite //
        && _type != type_none //
        && (_type == type_float //
        || _type == type_double //
        || _type == type_long_double);
    }

    bool is_numeric() const
    {
        return is_integer() || is_real();
    }

    expr_type type() const
    {
        return _type;
    }

    virtual bool append_expr(IExpr * elem)
    {
        return false;
    }

    virtual size_t length() const
    {
        return 0;
    }

#if 0
    virtual IExpr * get(size_t idx)
    {
        return NULL;
    }

    virtual const IExpr * get(size_t idx) const
    {
        return NULL;
    }
#endif

#if 0
    virtual bool rewind()
    {
        return false;
    }

    virtual bool has_next()
    {
        return false;
    }

    virtual IExpr * get_next()
    {
        return NULL;
    }
#endif

    virtual bool rewind() const
    {
        return false;
    }

    virtual bool has_next() const
    {
        return false;
    }

    virtual const IExpr * get_next() const
    {
        return NULL;
    }

    virtual std::string to_std_string() const = 0;

protected:

    IExpr * prev()
    {
        return _prev;
    }

    const IExpr * prev() const
    {
        return _prev;
    }

    const IExpr * next() const
    {
        return _next;
    }

    IExpr * next()
    {
        return _next;
    }

    void set_prev(IExpr * p)
    {
        _prev = p;
    }

    void set_next(IExpr * p)
    {
        _next = p;
    }

    void set_prev_and_next(IExpr * p, IExpr * n)
    {
        _prev = p;
        _next = n;
    }

    expr_type _type;
    IExpr * _prev;
    IExpr * _next;
};

} // namespace SolarWindLisp

#endif // _SOLAR_WIND_LISP_COMPOSITE_H_
