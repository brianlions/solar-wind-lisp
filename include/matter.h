/*
 * file name:           include/matter.h
 *
 * author:              Brian Yi ZHANG
 * email:               brianlions@gmail.com
 * date created:        Sat Nov 22 23:15:18 2014 CST
 */

#ifndef _SOLAR_WIND_LISP_MATTER_H_
#define _SOLAR_WIND_LISP_MATTER_H_

#include <string>
#include "types.h"

namespace SolarWindLisp
{

class MatterIF
{
public:
    enum matter_type_t
    {
        matter_atom = 0,
        matter_composite_expr,
        matter_prim_proc,
        matter_proc,
        matter_future,
        NUM_OF_MATTER_TYPES,
    };

    static const char * matter_type_name(matter_type_t t)
    {
        switch (t) {
            case matter_atom:
                return "atom";
            case matter_composite_expr:
                return "composite_expr";
            case matter_prim_proc:
                return "prim_proc";
            case matter_proc:
                return "proc";
            case matter_future:
                return "future";
            default:
                return "ANTIMATTER";
        }
    }

    virtual matter_type_t matter_type() const = 0;

    bool is_atom() const
    {
        return matter_atom == matter_type();
    }

    bool is_composite_expr() const
    {
        return matter_composite_expr == matter_type();
    }

    bool is_prim_proc() const
    {
        return matter_prim_proc == matter_type();
    }

    bool is_proc() const
    {
        return matter_proc == matter_type();
    }

    bool is_future() const
    {
        return matter_future == matter_type();
    }

    const char * matter_type_name() const
    {
        return MatterIF::matter_type_name(matter_type());
    }

    virtual ~MatterIF()
    {
    }

    static const char * DEFAULT_INDENT_SEQ;

    virtual std::string debug_string(bool compact = true, int level = 0,
            const char * indent_seq = DEFAULT_INDENT_SEQ) const = 0;
    virtual std::string to_string() const =0;
#if 0
    // TODO: implement!
    virtual std::string repr(bool compact = true, int level = 0) const = 0;
#endif
};

} // namespace SolarWindLisp

#endif // _SOLAR_WIND_LISP_MATTER_H_
