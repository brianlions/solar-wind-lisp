#include <new>
#include "swl_expr.h"

namespace SolarWindLisp
{

Expr * Expr::create(const char * buf, size_t length)
{
    Expr * result = new (std::nothrow) Expr();
    if (!result) {
        return NULL;
    }

    if (!buf) {
        return result;
    }

    if (result->parse(buf, length)) {
        return result;
    }

    delete result;
    return NULL;
}

bool Expr::parse(const char * buf, size_t length)
{
    if (parse_bool(buf, length) || parse_integer(buf, length)
            || parse_real(buf, length) || parse_cstr(buf, length)) {
        return true;
    }

    return false;
}

bool Expr::parse_bool(const char * buf, size_t length)
{
    if (length == 4 && !strcmp(buf, "true")) {
        _atom_type = atom_bool;
        _atom_data.num.b = true;
        return true;
    }

    if (length == 5 && !strcmp(buf, "false")) {
        _atom_type = atom_bool;
        _atom_data.num.b = false;
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

    if (!length) {
        return false;
    }

    if (length > 2 && (!strncmp(buf, "0x", 2) || !strncmp(buf, "0X", 2))) {
        base = 16;
    }
    else if (length > 1 && buf[0] == '0') {
        base = 8;
    }

    // decimal
    if (base == 10) {
        long int v = strtol(buf, &endptr, base);
        if (!errno && endptr == buf + length) {
            if (v >= std::numeric_limits < int32_t > ::min()
                    && v <= std::numeric_limits < int32_t > ::max()) {
                _atom_type = atom_i32;
                _atom_data.num.i32 = static_cast<int32_t>(v);
                errno = saved_errno;
                return true;
            }

            if (v >= std::numeric_limits < int64_t > ::min()
                    && v <= std::numeric_limits < int64_t > ::max()) {
                _atom_type = atom_i64;
                _atom_data.num.i64 = static_cast<int64_t>(v);
                errno = saved_errno;
                return true;
            }
        }

        return false;
    }

    // possibly (but NOT probably) hexdecimal or octal
    unsigned long v = strtoul(buf + ((base == 16) ? 2 : 1), &endptr, base);
    if (!errno && endptr == buf + length) {
        if (v >= std::numeric_limits < uint32_t > ::min()
                && v <= std::numeric_limits < uint32_t > ::max()) {
            _atom_type = atom_u32;
            _atom_data.num.u32 = static_cast<uint32_t>(v);
            errno = saved_errno;
            return true;
        }

        if (v >= std::numeric_limits < uint64_t > ::min()
                && v <= std::numeric_limits < uint64_t > ::max()) {
            _atom_type = atom_u64;
            _atom_data.num.u64 = static_cast<uint64_t>(v);
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

    if (!length) {
        return false;
    }

    double d = strtod(buf, &endptr);
    if (!errno && endptr == buf + length) {
        _atom_type = atom_double;
        _atom_data.num.d = d;
        errno = saved_errno;
        return true;
    }

    long double ld = strtold(buf, &endptr);
    if (!errno && endptr == buf + length) {
        _atom_type = atom_long_double;
        _atom_data.num.ld = ld;
        errno = saved_errno;
        return true;
    }

    return false;
}

bool Expr::parse_cstr(const char * buf, size_t length)
{
    if (_atom_data.set_string(buf, length)) {
        _atom_type = atom_cstr;
        return true;
    }
    return false;
}

#define macro_to_signed(name, T)                                        \
bool Expr::to_##name(T &v) const                                        \
{                                                                       \
    switch (_atom_type) {                                               \
        case atom_bool:                                                 \
            v = static_cast<T>(_atom_data.num.b);                       \
            return true;                                                \
        case atom_i32:                                                  \
            v = _atom_data.num.i32;                                     \
            return true;                                                \
        case atom_u32:                                                  \
            if (_atom_data.num.u32 > static_cast<uint32_t>(             \
                    std::numeric_limits<T>::max())) {                   \
                return false;                                           \
            } else {                                                    \
                v = static_cast<T>(_atom_data.num.u32);                 \
                return true;                                            \
            }                                                           \
        case atom_i64:                                                  \
            if (_atom_data.num.i64 > std::numeric_limits<T>::max()) {   \
                return false;                                           \
            } else {                                                    \
                v = static_cast<T>(_atom_data.num.i64);                 \
                return true;                                            \
            }                                                           \
        case atom_u64:                                                  \
            if (_atom_data.num.u64 > static_cast<uint64_t>(             \
                    std::numeric_limits<T>::max())) {                   \
                return false;                                           \
            } else {                                                    \
                v = static_cast<T>(_atom_data.num.u64);                 \
                return true;                                            \
            }                                                           \
        case atom_float:                                                \
            if (_atom_data.num.f > std::numeric_limits<T>::max()) {     \
                return false;                                           \
            } else {                                                    \
                v = static_cast<T>(_atom_data.num.f);                   \
                return true;                                            \
            }                                                           \
        case atom_double:                                               \
            if (_atom_data.num.d > std::numeric_limits<T>::max()) {     \
                return false;                                           \
            } else {                                                    \
                v = static_cast<T>(_atom_data.num.d);                   \
                return true;                                            \
            }                                                           \
        case atom_long_double:                                          \
            if (_atom_data.num.ld > std::numeric_limits<T>::max()) {    \
                return false;                                           \
            } else {                                                    \
                v = static_cast<T>(_atom_data.num.ld);                  \
                return true;                                            \
            }                                                           \
        case atom_cstr:                                                 \
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
    switch (_atom_type) {                                               \
        case atom_bool:                                                 \
            v = static_cast<T>(_atom_data.num.b);                       \
            return true;                                                \
        case atom_i32:                                                  \
            if (_atom_data.num.i32 < 0) {                               \
                return false;                                           \
            } else {                                                    \
                v = static_cast<T>(_atom_data.num.i32);                 \
                return true;                                            \
            }                                                           \
        case atom_u32:                                                  \
            v = _atom_data.num.u32;                                     \
            return true;                                                \
        case atom_i64:                                                  \
            if (_atom_data.num.i64 < 0 || static_cast<uint64_t>(        \
                    _atom_data.num.i64) > std::numeric_limits<T>::max()) { \
                return false;                                           \
            } else {                                                    \
                v = static_cast<T>(_atom_data.num.i64);                 \
                return true;                                            \
            }                                                           \
        case atom_u64:                                                  \
            if (_atom_data.num.u64 > std::numeric_limits<T>::max()) {   \
                return false;                                           \
            } else {                                                    \
                v = static_cast<T>(_atom_data.num.u64);                 \
                return true;                                            \
            }                                                           \
        case atom_float:                                                \
            if (_atom_data.num.f > std::numeric_limits<T>::max()) {     \
                return false;                                           \
            } else {                                                    \
                v = static_cast<T>(_atom_data.num.f);                   \
                return true;                                            \
            }                                                           \
        case atom_double:                                               \
            if (_atom_data.num.d > std::numeric_limits<T>::max()) {     \
                return false;                                           \
            } else {                                                    \
                v = static_cast<T>(_atom_data.num.d);                   \
                return true;                                            \
            }                                                           \
        case atom_long_double:                                          \
            if (_atom_data.num.ld > std::numeric_limits<T>::max()) {    \
                return false;                                           \
            } else {                                                    \
                v = static_cast<T>(_atom_data.num.ld);                  \
                return true;                                            \
            }                                                           \
        case atom_cstr:                                                 \
        default:                                                        \
            return false;                                               \
    }                                                                   \
}
macro_to_unsigned(u32, uint32_t)
macro_to_unsigned(u64, uint64_t)
#undef macro_to_unsigned

bool Expr::to_double(double &v) const
{
    // FIXME:
    switch (_atom_type) {
        case atom_bool:
            v = static_cast<double>(_atom_data.num.b);
            return true;
        case atom_i32:
            v = static_cast<double>(_atom_data.num.i32);
            return true;
        case atom_u32:
            v = static_cast<double>(_atom_data.num.u32);
            return true;
        case atom_i64:
            v = static_cast<double>(_atom_data.num.i64);
            return true;
        case atom_u64:
            v = static_cast<double>(_atom_data.num.u64);
            return true;
        case atom_double:
            v = static_cast<double>(_atom_data.num.d);
            return true;
        case atom_long_double:
            v = static_cast<double>(_atom_data.num.ld);
            return true;
        case atom_cstr:
            return false;
        default:
            return false;
    }
}

bool Expr::to_long_double(long double &v) const
{
    // FIXME:
    switch (_atom_type) {
        case atom_bool:
            v = static_cast<long double>(_atom_data.num.b);
            return true;
        case atom_i32:
            v = static_cast<long double>(_atom_data.num.i32);
            return true;
        case atom_u32:
            v = static_cast<long double>(_atom_data.num.u32);
            return true;
        case atom_i64:
            v = static_cast<long double>(_atom_data.num.i64);
            return true;
        case atom_u64:
            v = static_cast<long double>(_atom_data.num.u64);
            return true;
        case atom_double:
            v = static_cast<long double>(_atom_data.num.d);
            return true;
        case atom_long_double:
            v = static_cast<long double>(_atom_data.num.ld);
            return true;
        case atom_cstr:
            return false;
        default:
            return false;
    }
}

std::string Expr::debug_string(bool compact, int level,
        const char * indent_seq) const
{
    std::string indent = compact ? "" : Utils::repeat(indent_seq, level);
    std::string first_sep =
            compact ? "" : (std::string("\n") + indent + indent_seq);
    std::string sep =
            compact ? "," : (std::string(",\n") + indent + indent_seq);

    std::stringstream ss;
    ss << indent << "Expr{" << first_sep //
        << "prev:" << (compact ? "" : " ") << ((void *) _prev) << sep //
        << "this:" << (compact ? "" : " ") << ((void *) this) << sep //
        << "type:" << (compact ? "" : " ") << atom_type_name(_atom_type) << sep //
        << "data:" << (compact ? "" : " ");
    if (is_bool()) {
        ss << ((_atom_data.num.b) ? "true" : "false");
    }
    else if (is_i32() || is_i64()) {
        int64_t v;
        to_i64(v);
        ss << v;
    }
    else if (is_u32() || is_u64()) {
        uint64_t v;
        to_u64(v);
        ss << v;
    }
    else if (is_real()) {
        long double ld;
        to_long_double(ld);
        ss << ld;
    }
    else if (is_cstr()) {
        ss << "`" << to_cstr() << "'";
    }
    ss << "}";
    return ss.str();
}

CompositeExpr * CompositeExpr::create()
{
    CompositeExpr * result = new (std::nothrow) CompositeExpr();
    return result;
}

bool CompositeExpr::append_expr(IExpr * expr)
{
    if (!_head) {
        _head = _tail = expr;
    }
    else {
        expr->set_prev_and_next(_tail, NULL);
        _tail->set_next(expr);
        _tail = expr;
    }
    _size++;
    return true;
}

bool CompositeExpr::rewind() const
{
    if (_head) {
        _cursor = _head;
        return true;
    }

    return false;
}

bool CompositeExpr::rewind()
{
    if (_head) {
        _cursor = _head;
        return true;
    }

    return false;
}

std::string CompositeExpr::debug_string(bool compact, int level,
        const char * indent_seq) const
{
    std::string indent = compact ? "" : Utils::repeat(indent_seq, level);
    std::string first_sep =
            compact ? "" : (std::string("\n") + indent + indent_seq);
    std::string sep =
            compact ? "," : (std::string(",\n") + indent + indent_seq);

    std::stringstream ss;
    ss << indent << "CompositeExpr{" << first_sep //
        << "head:" << (compact ? "" : " ") << ((void *) _head) << sep //
        << "tail:" << (compact ? "" : " ") << ((void *) _tail) << sep //
        << "size:" << (compact ? "" : " ") << _size << sep //
        << "elem:" << (compact ? "[" : " [");
    if (_size > 0) {
        ss << (compact ? "" : "\n");
        const IExpr * e = _head;
        while (e) {
            ss << e->debug_string(compact, level + 2, indent_seq);
            e = e->next();
            if (e) {
                ss << (compact ? "," : ",\n");
            }
        }
    }
    else {
        ss << (compact ? "" : " ");
    }
    ss << "]}";
    return ss.str();
}

} // namespace SolarWindLisp
