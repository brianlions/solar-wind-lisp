#ifndef _SOLAR_WIND_LISTP_PRIM_PROC_H_
#define _SOLAR_WIND_LISTP_PRIM_PROC_H_

#include "swl_matter_if.h"

namespace SolarWindLisp
{

class PrimProc: public IMatter
{
public:
    matter_type_t matter_type() const
    {
        return matter_prim_proc;
    }
};

} // namespace SolarWindLisp

#endif // _SOLAR_WIND_LISTP_PRIM_PROC_H_
