#include "swl_component.h"

namespace SolarWindLisp
{

const char * IExpr::DEFAULT_INDENT_SEQ = "    ";

const char * IExpr::type_name(expr_type t)
{
    switch (t)
    {
        case type_none:
            return "none";
        case type_bool:
            return "b";
        case type_i32:
            return "i32";
        case type_u32:
            return "u32";
        case type_i64:
            return "i64";
        case type_u64:
            return "u64";
        case type_float:
            return "f";
        case type_double:
            return "d";
        case type_long_double:
            return "ld";
        case type_string:
            return "str";
        case type_composite:
            return "composite";
        default:
            return "UNKNOWN";
    }
}

} // namespace SolarWindLisp
