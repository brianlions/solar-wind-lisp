#ifndef _SOLAR_WIND_LISP_EXPR_H_
#define _SOLAR_WIND_LISP_EXPR_H_

#include <sstream>
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

    static Expr * create(const char * buf, size_t length);

    bool parse(const char * buf, size_t length);
    bool parse_none(const char * buf, size_t length);
    bool parse_bool(const char * buf, size_t length);
    bool parse_integer(const char * buf, size_t length);
    bool parse_real(const char * buf, size_t length);
    bool parse_string(const char * buf, size_t length);

    bool to_i32(int32_t &v) const;
    bool to_i64(int64_t &v) const;
    bool to_u32(uint32_t &v) const;
    bool to_u64(uint64_t &v) const;
    bool to_double(double &v) const;
    bool to_long_double(long double &v) const;
    const char * to_string() const
    {
        return is_string() ? _data.str.buffer : NULL;
    }

#if 0
    bool to_i32_unsafe(int32_t &v) const;
    bool to_i64_unsafe(int64_t &v) const;
    bool to_u32_unsafe(uint32_t &v) const;
    bool to_u64_unsafe(uint64_t &v) const;
    bool to_double_unsafe(double &v) const;
    bool to_long_double_unsafe(long double &v) const;
#endif

    struct AtomData
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

        bool set_string(const char * buf, size_t length)
        {
            if (str.buffer) {
                // reuse existing buffer if long enough
                if (str.size >= length + 1) {
                    memcpy(str.buffer, buf, length + 1);
                    str.length = length;
                    return true;
                }

                free(str.buffer);
                str.buffer = NULL;
                str.length = 0;
                str.size = 0;
            }

            if (!(str.buffer = strdup(buf))) {
                return false;
            }

            str.size = length + 1;
            str.length = length;
            return true;
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
            size_t length;
            size_t size;
        } str;
    };

    std::string to_std_string() const
    {
        std::stringstream ss;
        ss << "Expr{prev:" << ((void *) _prev)
            << ",this:" << ((void *) this)
            << ",next:" << ((void *) _next)
            << ",type:" << _type
            << ",data:";
        if (is_none()) {
            ss << "none";
        }
        else if (is_integer()) {
            int64_t v;
            to_i64(v);
            ss << v;
        }
        else if (is_real()) {
            double d;
            to_double(d);
            ss << d;
        }
        else if (is_string()) {
            ss << "`" << to_string() << "'";
        }
        ss << "}";
        return ss.str();
    }

private:
    AtomData _data;
};

class CompositeExpr: public IExpr
{
public:
#define type_test_macro2(name, value)   \
bool is_##name() const {                \
    return value;                       \
}
    type_test_macro2(none,        false)
    type_test_macro2(bool,        false)
    type_test_macro2(i32,         false)
    type_test_macro2(u32,         false)
    type_test_macro2(i64,         false)
    type_test_macro2(u64,         false)
    type_test_macro2(float,       false)
    type_test_macro2(double,      false)
    type_test_macro2(long_double, false)
    type_test_macro2(string,      false)
    type_test_macro2(composite,   true)
#undef type_test_macro2

    CompositeExpr() :
            _head(NULL), _tail(NULL), _cursor(NULL), _size(0)
    {
    }

    ~CompositeExpr()
    {
        _destroy();
    }

    bool append_expr(IExpr * expr);

    size_t length() const
    {
        return _size;
    }

    bool rewind() const;

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

    std::string to_std_string() const
    {
        std::stringstream ss;
        ss << "CompositeExpr{head:" << ((void *) _head)
            << ",tail:" << ((void *) _tail)
            << ",size:" << _size
            << ",elem:[" << std::endl;
        const IExpr * e = _head;
        while (e) {
            ss << "    " << e->to_std_string() << std::endl;
            e = e->next();
        }
        ss << "]}";
        return ss.str();
    }

private:

    void _destroy()
    {
        while (_head) {
            IExpr * p = _head;
            _head = _head->next();
            delete p;
        }

        _head = _tail = _cursor = NULL;
        _size = 0;
    }

    IExpr * _head;
    IExpr * _tail;
    mutable IExpr * _cursor;
    size_t _size;
};

} // namespace SolarWindLisp

#endif // _SOLAR_WIND_LISP_EXPR_H_
