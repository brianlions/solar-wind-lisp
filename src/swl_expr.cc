#include "swl_expr.h"

namespace SolarWindLisp
{

Expr * Expr::create(const char * buf, size_t length)
{
    Expr * result = new Expr();
    if (result->parse(buf, length)) {
        return result;
    }

    delete result;
    return NULL;
}

bool Expr::parse(const char * buf, size_t length)
{
    if (parse_none(buf, length) || parse_bool(buf, length)
            || parse_integer(buf, length) || parse_real(buf, length)
            || parse_string(buf, length))
    {
        return true;
    }

    return false;
}

bool Expr::parse_none(const char * buf, size_t length)
{
    if (length == 4 && !strcmp(buf, "none"))
    {
        _type = type_none;
        return true;
    }

    return false;
}

bool Expr::parse_bool(const char * buf, size_t length)
{
    if (length == 4 && !strcmp(buf, "true"))
    {
        _type = type_bool;
        _data.num.b = true;
        return true;
    }

    if (length == 5 && !strcmp(buf, "false"))
    {
        _type = type_bool;
        _data.num.b = false;
        return true;
    }

    return false;
}

bool Expr::parse_integer(const char * buf, size_t length)
{
    int base = 10;
    char * endptr = NULL;
    int saved_errno = errno;
    errno = 0;

    if (!length)
    {
        return false;
    }

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

bool Expr::parse_real(const char * buf, size_t length)
{
    char * endptr = NULL;
    int saved_errno = errno;
    errno = 0;

    if (!length)
    {
        return false;
    }

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

bool Expr::parse_string(const char * buf, size_t length)
{
    if (_data.set_string(buf, length)) {
        _type = type_string;
        return true;
    }
    return false;
}

#define macro_to_signed(name, T)                                        \
bool Expr::to_##name(T &v) const                                        \
{                                                                       \
    switch (_type) {                                                    \
        case type_none:                                                 \
            v = 0;                                                      \
            return true;                                                \
        case type_bool:                                                 \
            v = static_cast<T>(_data.num.b);                            \
            return true;                                                \
        case type_i32:                                                  \
            v = _data.num.i32;                                          \
            return true;                                                \
        case type_u32:                                                  \
            if (_data.num.u32 > static_cast<uint32_t>(                  \
                    std::numeric_limits<T>::max())) {                   \
                return false;                                           \
            } else {                                                    \
                v = static_cast<T>(_data.num.u32);                      \
                return true;                                            \
            }                                                           \
        case type_i64:                                                  \
            if (_data.num.i64 > std::numeric_limits<T>::max()) {        \
                return false;                                           \
            } else {                                                    \
                v = static_cast<T>(_data.num.i64);                      \
                return true;                                            \
            }                                                           \
        case type_u64:                                                  \
            if (_data.num.u64 > static_cast<uint64_t>(                  \
                    std::numeric_limits<T>::max())) {                   \
                return false;                                           \
            } else {                                                    \
                v = static_cast<T>(_data.num.u64);                      \
                return true;                                            \
            }                                                           \
        case type_float:                                                \
            if (_data.num.f > std::numeric_limits<T>::max()) {          \
                return false;                                           \
            } else {                                                    \
                v = static_cast<T>(_data.num.f);                        \
                return true;                                            \
            }                                                           \
        case type_double:                                               \
            if (_data.num.d > std::numeric_limits<T>::max()) {          \
                return false;                                           \
            } else {                                                    \
                v = static_cast<T>(_data.num.d);                        \
                return true;                                            \
            }                                                           \
        case type_long_double:                                          \
            if (_data.num.ld > std::numeric_limits<T>::max()) {         \
                return false;                                           \
            } else {                                                    \
                v = static_cast<T>(_data.num.ld);                       \
                return true;                                            \
            }                                                           \
        case type_string:                                               \
        case type_composite:                                            \
        default:                                                        \
            return false;                                               \
    }                                                                   \
}
macro_to_signed(i32, int32_t)
macro_to_signed(i64, int64_t)
#undef macro_to_signed

#define macro_to_unsigned(name, T)                                      \
bool Expr::to_##name(T &v) const                                        \
{                                                                       \
    switch (_type) {                                                    \
        case type_none:                                                 \
            v = 0;                                                      \
            return true;                                                \
        case type_bool:                                                 \
            v = static_cast<T>(_data.num.b);                            \
            return true;                                                \
        case type_i32:                                                  \
            if (_data.num.i32 < 0) {                                    \
                return false;                                           \
            } else {                                                    \
                v = static_cast<T>(_data.num.i32);                      \
                return true;                                            \
            }                                                           \
        case type_u32:                                                  \
            v = _data.num.u32;                                          \
            return true;                                                \
        case type_i64:                                                  \
            if (_data.num.i64 < 0 || static_cast<uint64_t>(             \
                    _data.num.i64) > std::numeric_limits<T>::max()) {   \
                return false;                                           \
            } else {                                                    \
                v = static_cast<T>(_data.num.i64);                      \
                return true;                                            \
            }                                                           \
        case type_u64:                                                  \
            if (_data.num.u64 > std::numeric_limits<T>::max()) {        \
                return false;                                           \
            } else {                                                    \
                v = static_cast<T>(_data.num.u64);                      \
                return true;                                            \
            }                                                           \
        case type_float:                                                \
            if (_data.num.f > std::numeric_limits<T>::max()) {          \
                return false;                                           \
            } else {                                                    \
                v = static_cast<T>(_data.num.f);                        \
                return true;                                            \
            }                                                           \
        case type_double:                                               \
            if (_data.num.d > std::numeric_limits<T>::max()) {          \
                return false;                                           \
            } else {                                                    \
                v = static_cast<T>(_data.num.d);                        \
                return true;                                            \
            }                                                           \
        case type_long_double:                                          \
            if (_data.num.ld > std::numeric_limits<T>::max()) {         \
                return false;                                           \
            } else {                                                    \
                v = static_cast<T>(_data.num.ld);                       \
                return true;                                            \
            }                                                           \
        case type_string:                                               \
        case type_composite:                                            \
        default:                                                        \
            return false;                                               \
    }                                                                   \
}
macro_to_unsigned(u32, uint32_t)
macro_to_unsigned(u64, uint64_t)
#undef macro_to_unsigned

bool Expr::to_double(double &v) const
{
    return false;
}

bool Expr::to_long_double(long double &v) const
{
    return false;
}

bool CompositeExpr::append_expr(IExpr * expr)
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

bool CompositeExpr::rewind() const
{
    if (_head)
    {
        _cursor = _head;
        return true;
    }

    return false;
}

} // namespace SolarWindLisp
