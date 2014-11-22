#ifndef _SOLAR_WIND_LISP_FUTURE_H_
#define _SOLAR_WIND_LISP_FUTURE_H_

#include "swl_matter_if.h"

namespace SolarWindLisp
{

class Future: public IMatter
{
public:
    matter_type_t matter_type() const
    {
        return matter_future;
    }
};

} // namespace SolarWindLisp

#endif // _SOLAR_WIND_LISP_FUTURE_H_
