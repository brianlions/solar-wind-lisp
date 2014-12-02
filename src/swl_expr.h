#ifndef _SOLAR_WIND_LISP_EXPR_H_
#define _SOLAR_WIND_LISP_EXPR_H_

#include <string.h>
#include <stdlib.h>
#include <sstream>
#include <deque>

#include "swl_matter_if.h"
#include "swl_utils.h"

namespace SolarWindLisp
{
// TODO: rename to Atom
class Expr: public IMatter
{
public:
    enum atom_type_t
    {
        atom_bool = 0,
        atom_i32,
        atom_u32,
        atom_i64,
        atom_u64,
        atom_double,
        atom_long_double,
        atom_cstr,
        NUM_OF_ATOM_TYPES
    };

    static const char * atom_type_name(atom_type_t t);

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
    atom_type_test_macro(double) //
    atom_type_test_macro(long_double) //
    atom_type_test_macro(cstr) //
#undef atom_type_test_macro

    bool is_quoted_cstr() const
    {
        return _atom_type == atom_cstr && _atom_data.str.quoted;
    }

    bool is_integer() const
    {
        return _atom_type == atom_bool || _atom_type == atom_i32
                || _atom_type == atom_u32 || _atom_type == atom_i64
                || _atom_type == atom_u64;
    }

    bool is_real() const
    {
        return _atom_type == atom_double || _atom_type == atom_long_double;
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

    static ExprPtr create(const char * buf = NULL, size_t length = 0);

    bool parse(const char * buf, size_t length);
    bool parse_bool(const char * buf, size_t length);
    bool parse_integer(const char * buf, size_t length);
    bool parse_real(const char * buf, size_t length);
    bool parse_cstr(const char * buf, size_t length);

    void set_bool(bool v);
    void set_i32(int32_t v);
    void set_u32(uint32_t v);
    void set_i64(int64_t v);
    void set_u64(uint64_t v);
    void set_double(double v);
    void set_long_double(long double v);

    bool to_bool(bool &v) const;
    bool to_i32(int32_t &v) const;
    bool to_u32(uint32_t &v) const;
    bool to_i64(int64_t &v) const;
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

    bool not_empty() const;

    struct AtomData
    {
        static const char C_SQ = '\'';
        static const char C_DQ = '\"';

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
            str.quoted = false;
        }

        bool set_string(const char * buf, size_t length);

        union
        {
            bool b;
            int32_t i32;
            uint32_t u32;
            int64_t i64;
            uint64_t u64;
            double d;
            long double ld;
        } num;

        struct
        {
            char * buffer;
            size_t length;
            size_t size;
            bool quoted;
        } str;
    };

    std::string debug_string(bool compact = true, int level = 0,
            const char * indent_seq = IMatter::DEFAULT_INDENT_SEQ) const;

private:
    Expr() :
            _atom_type(atom_bool)
    {
    }

    atom_type_t _atom_type;
    AtomData _atom_data;
};

class CompositeExpr: public IMatter
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

    static CompositeExprPtr create();

    bool append_expr(MatterPtr expr);

    size_t size() const
    {
        return _items.size();
    }

    bool rewind() const
    {
        _cursor = 0;
        return true;
    }

    bool has_next() const
    {
        return _cursor < _items.size();
    }

    MatterPtr get_next() const
    {
        return (_cursor < _items.size()) ? _items[_cursor++] : MatterPtr();
    }

    MatterPtr get(size_t idx) const
    {
        return (idx < _items.size()) ? _items[idx] : MatterPtr();
    }

    std::string debug_string(bool compact = true, int level = 0,
            const char * indent_seq = IMatter::DEFAULT_INDENT_SEQ) const;

private:
    static const size_t _INITIAL_CAPACITY = 10;
    static const size_t _CAPACITY_DELTA = 10;

    CompositeExpr() :
        _cursor(0)
    {
    }

    void _destroy()
    {
        // FIXME delete element in `_items'
    }

    std::deque<MatterPtr> _items;
    mutable size_t _cursor;
};

} // namespace SolarWindLisp

#endif // _SOLAR_WIND_LISP_EXPR_H_
