#ifndef _SOLAR_WIND_LIST_UTILS_H_
#define _SOLAR_WIND_LIST_UTILS_H_

#include <stdio.h>

#include <sstream>
#include <string>

#ifndef array_size
#define array_size(a) (sizeof(a) / sizeof(a[0]))
#endif

namespace SolarWindLisp
{

/*
 * Use class (instead of namespace) and static methods to enforce every
 * reference to any of these methods are prefixed by the class name.
 */
class Utils
{
public:
    static std::string repeat(const char * pat, int times)
    {
        std::stringstream ss;
        for (int i = 0; i < times; ++i) {
            ss << pat;
        }
        return ss.str();
    }
};

} // namespace SolarWindLisp

#endif // _SOLAR_WIND_LIST_UTILS_H_
