#ifndef _SOLAR_WIND_LISP_EXPR_H_
#define _SOLAR_WIND_LISP_EXPR_H_

#include <sstream>

#include "swl_matter_if.h"
#include "swl_expr_if.h"
#include "swl_utils.h"

namespace SolarWindLisp
{
// TODO: rename to Atom
class Expr: public IExpr
{
public:
    enum atom_type_t
    {
        atom_bool = 0,
        atom_i32,
        atom_u32,
        atom_i64,
        atom_u64,
        atom_float,
        atom_double,
        atom_long_double,
        atom_cstr,
        NUM_OF_ATOM_TYPES
    };

    static const char * atom_type_name(atom_type_t t)
    {
        switch (t) {
            case atom_bool:
                return "bool";
            case atom_i32:
                return "i32";
            case atom_u32:
                return "u32";
            case atom_i64:
                return "i64";
            case atom_u64:
                return "u64";
            case atom_float:
                return "float";
            case atom_double:
                return "d";
            case atom_long_double:
                return "ld";
            case atom_cstr:
                return "cstr";
            default:
                return "ANTIMATTER";
        }
    }

    const char * atom_type_name() const
    {
        return atom_type_name(_atom_type);
    }

#define atom_type_test_macro(name)      \
bool is_##name() const {                \
    return _atom_type == atom_##name;   \
}
    atom_type_test_macro(bool) //
    atom_type_test_macro(i32) //
    atom_type_test_macro(u32) //
    atom_type_test_macro(i64) //
    atom_type_test_macro(u64) //
    atom_type_test_macro(float) //
    atom_type_test_macro(double) //
    atom_type_test_macro(long_double) //
    atom_type_test_macro(cstr) //
#undef atom_type_test_macro

    bool is_integer() const
    {
        return _atom_type == atom_bool || _atom_type == atom_i32
                || _atom_type == atom_u32 || _atom_type == atom_i64
                || _atom_type == atom_u64;
    }

    bool is_real() const
    {
        return _atom_type == atom_float || _atom_type == atom_double
                || _atom_type == atom_long_double;
    }

    bool is_numeric() const
    {
        return is_integer() || is_real();
    }

    matter_type_t matter_type() const
    {
        return matter_atom;
    }

    atom_type_t atom_type() const
    {
        return _atom_type;
    }

    virtual ~Expr()
    {
    }

    static Expr * create(const char * buf = NULL, size_t length = 0);

    bool parse(const char * buf, size_t length);
    bool parse_bool(const char * buf, size_t length);
    bool parse_integer(const char * buf, size_t length);
    bool parse_real(const char * buf, size_t length);
    bool parse_cstr(const char * buf, size_t length);

    bool to_i32(int32_t &v) const;
    bool to_i64(int64_t &v) const;
    bool to_u32(uint32_t &v) const;
    bool to_u64(uint64_t &v) const;
    bool to_double(double &v) const;
    bool to_long_double(long double &v) const;
    const char * to_cstr() const
    {
        return is_cstr() ? _atom_data.str.buffer : NULL;
    }

#if 0
    // TODO: unsafe (overflow or underflow) but faster
    bool to_i32_unsafe(int32_t &v) const;
    bool to_i64_unsafe(int64_t &v) const;
    bool to_u32_unsafe(uint32_t &v) const;
    bool to_u64_unsafe(uint64_t &v) const;
    bool to_double_unsafe(double &v) const;
    bool to_long_double_unsafe(long double &v) const;
#endif

    struct AtomData
    {
        AtomData()
        {
            memset(&num, 0, sizeof(num));
            memset(&str, 0, sizeof(str));
        }

        ~AtomData()
        {
            if (str.buffer) {
                free(str.buffer);
            }
        }

        void reset()
        {
            memset(&num, 0, sizeof(num));
            str.length = 0;
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

    std::string debug_string(bool compact = true, int level = 0,
            const char * indent_seq = IMatter::DEFAULT_INDENT_SEQ) const;

private:
    /* Uses the create() to initialize new instance. */
    Expr() :
            _atom_type(atom_bool)
    {
    }

    atom_type_t _atom_type;
    AtomData _atom_data;
};

class CompositeExpr: public IExpr
{
public:
    ~CompositeExpr()
    {
        _destroy();
    }

    matter_type_t matter_type() const
    {
        return matter_molecule;
    }

    static CompositeExpr * create();

    bool append_expr(IExpr * expr);

    size_t size() const
    {
        return _size;
    }

#if 0
    // TODO: implement
    virtual IExpr * get(size_t idx)
    {
        return NULL;
    }

    virtual const IExpr * get(size_t idx) const
    {
        return NULL;
    }
#endif

    bool rewind();

    bool has_next()
    {
        return _cursor != NULL;
    }

    IExpr * get_next()
    {
        IExpr * r = _cursor;
        _cursor = _cursor->next();
        return r;
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

    std::string debug_string(bool compact = true, int level = 0,
            const char * indent_seq = IMatter::DEFAULT_INDENT_SEQ) const;

private:
    /* Uses the create() to initialize new instance. */
    CompositeExpr() :
            _head(NULL), _tail(NULL), _cursor(NULL), _size(0)
    {
    }

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
