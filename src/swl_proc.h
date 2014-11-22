#ifndef _SOLAR_WIND_LISP_PROC_H_
#define _SOLAR_WIND_LISP_PROC_H_

#include "swl_matter_if.h"

namespace SolarWindLisp
{

class Proc: public IMatter
{
public:
    matter_type_t matter_type() const
    {
        return matter_proc;
    }
};

} // namespace SolarWindLisp

#endif // _SOLAR_WIND_LISP_PROC_H_
