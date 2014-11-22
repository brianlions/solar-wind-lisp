#ifndef _SOLAR_WIND_LISP_MATTER_IF_H_
#define _SOLAR_WIND_LISP_MATTER_IF_H_

#include <string>

namespace SolarWindLisp
{

class IMatter
{
public:
    enum matter_type_t
    {
        matter_atom = 0,
        matter_molecule,
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
            case matter_molecule:
                return "molecule";
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

    bool is_molecule() const
    {
        return matter_molecule == matter_type();
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
        return IMatter::matter_type_name(matter_type());
    }

    virtual ~IMatter()
    {
    }

    static const char * DEFAULT_INDENT_SEQ;

    virtual std::string debug_string(bool compact = true, int level = 0,
            const char * indent_seq = DEFAULT_INDENT_SEQ) const = 0;
#if 0
    // TODO: implement!
    virtual std::string repr(bool compact = true, int level = 0) const = 0;
#endif
};

} // namespace SolarWindLisp

#endif // _SOLAR_WIND_LISP_MATTER_IF_H_
