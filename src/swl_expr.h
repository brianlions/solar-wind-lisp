#ifndef _SOLAR_WIND_LISP_EXPR_H_
#define _SOLAR_WIND_LISP_EXPR_H_

#include "swl_component.h"

namespace SolarWindLisp
{

class Expr: public IExpr
{
public:
#define type_test_macro(name)           \
bool is_##name() const {                \
    return _type == type_##name;        \
}
    type_test_macro(none)
    type_test_macro(bool)
    type_test_macro(i32)
    type_test_macro(u32)
    type_test_macro(i64)
    type_test_macro(u64)
    type_test_macro(float)
    type_test_macro(double)
    type_test_macro(long_double)
    type_test_macro(string)
    type_test_macro(composite)
#undef type_test_macro

    Expr()
    {
        _data.init();
    }

    virtual ~Expr()
    {
        _data.destroy();
    }

    bool parse(const char * buf, size_t length)
    {
        if (parse_none(buf, length) || parse_bool(buf, length)
                || parse_integer(buf, length) || parse_real(buf, length))
        {
            return true;
        }

        return false;
    }

    bool parse_none(const char * buf, size_t length)
    {
        if (!strcmp(buf, "none"))
        {
            _type = type_none;
            return true;
        }

        return false;
    }

    bool parse_bool(const char * buf, size_t length)
    {
        if (!strcmp(buf, "true"))
        {
            _type = type_bool;
            _data.num.b = true;
            return true;
        }

        if (!strcmp(buf, "false"))
        {
            _type = type_bool;
            _data.num.b = false;
            return true;
        }

        return false;
    }

    bool parse_integer(const char * buf, size_t length)
    {
        int base = 10;
        char * endptr = NULL;
        int saved_errno = errno;
        errno = 0;

        if (length > 2 && (!strncmp(buf, "0x", 2) || !strncmp(buf, "0X", 2)))
        {
            base = 16;
        }
        else if (length > 1 && buf[0] == '0')
        {
            base = 8;
        }

        // decimal
        if (base == 10)
        {
            long int v = strtol(buf, &endptr, base);
            if (!errno && endptr == buf + length)
            {
                if (v >= std::numeric_limits < int32_t > ::min()
                        && v <= std::numeric_limits < int32_t > ::max())
                {
                    _type = type_i32;
                    _data.num.i32 = static_cast<int32_t>(v);
                    errno = saved_errno;
                    return true;
                }

                if (v >= std::numeric_limits < int64_t > ::min()
                        && v <= std::numeric_limits < int64_t > ::max())
                {
                    _type = type_i64;
                    _data.num.i64 = static_cast<int64_t>(v);
                    errno = saved_errno;
                    return true;
                }
            }

            return false;
        }

        // possibly (but NOT probably) hexdecimal or octal
        unsigned long v = strtoul(buf + ((base == 16) ? 2 : 1), &endptr, base);
        if (!errno && endptr == buf + length)
        {
            if (v >= std::numeric_limits < uint32_t > ::min()
                    && v <= std::numeric_limits < uint32_t > ::max())
            {
                _type = type_u32;
                _data.num.u32 = static_cast<uint32_t>(v);
                errno = saved_errno;
                return true;
            }

            if (v >= std::numeric_limits < uint64_t > ::min()
                    && v <= std::numeric_limits < uint64_t > ::max())
            {
                _type = type_u64;
                _data.num.u64 = static_cast<uint64_t>(v);
                errno = saved_errno;
                return true;
            }
        }

        return false;
    }

    bool parse_real(const char * buf, size_t length)
    {
        char * endptr = NULL;
        int saved_errno = errno;
        errno = 0;

        double d = strtod(buf, &endptr);
        if (!errno && endptr == buf + length)
        {
            _type = type_double;
            _data.num.d = d;
            errno = saved_errno;
            return true;
        }

        long double ld = strtold(buf, &endptr);
        if (!errno && endptr == buf + length)
        {
            _type = type_long_double;
            _data.num.ld = ld;
            errno = saved_errno;
            return true;
        }

        return false;
    }

    struct AtomNode
    {
        void destroy()
        {
            if (str.buffer)
            {
                free(str.buffer);
            }
        }

        bool init()
        {
            memset(&num, 0, sizeof(num));
            memset(&str, 0, sizeof(str));
            return true;
        }

        void reset()
        {
        }

        union
        {
            bool b;
            int32_t i32;
            uint32_t u32;
            int64_t i64;
            uint64_t u64;
            float f;
            double d;
            long double ld;
        } num;

        struct
        {
            char * buffer;
            size_t used;
            size_t size;
        } str;
    };

private:
    AtomNode _data;
};

class CompositeExpr: public IExpr
{
public:
#define type_test_macro2(name, value)   \
bool is_##name() const {                \
    return value;                       \
}
    type_test_macro2(none, false)
    type_test_macro2(bool, false)
    type_test_macro2(i32, false)
    type_test_macro2(u32, false)
    type_test_macro2(i64, false)
    type_test_macro2(u64, false)
    type_test_macro2(float, false)
    type_test_macro2(double, false)
    type_test_macro2(long_double, false)
    type_test_macro2(string, false)
    type_test_macro2(composite, true)
#undef type_test_macro2

    CompositeExpr() :
            _head(NULL), _tail(NULL), _cursor(NULL), _size(0)
    {
    }

    ~CompositeExpr()
    {
        // FIXME: relinquish _head to _tail
    }

    bool append_expr(IExpr * expr)
    {
        if (!_head)
        {
            _head = _tail = expr;
        }
        else
        {
            expr->set_prev_and_next(_tail, NULL);
            _tail->set_next(expr);
            _tail = expr;
        }
        _size++;
        return true;
    }

    size_t length() const
    {
        return _size;
    }

    bool rewind() const
    {
        if (_head)
        {
            _cursor = _head;
            return true;
        }

        return false;
    }

    bool has_next() const
    {
        return _cursor != NULL;
    }

    const IExpr * get_next() const
    {
        const IExpr * r = _cursor;
        _cursor = _cursor->next();
        return r;
    }

private:
    IExpr * _head;
    IExpr * _tail;
    mutable IExpr * _cursor;
    size_t _size;
};

} // namespace SolarWindLisp

#endif // _SOLAR_WIND_LISP_EXPR_H_
