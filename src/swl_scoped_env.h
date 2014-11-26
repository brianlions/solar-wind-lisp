#ifndef _SOLAR_WIND_LISP_SCOPED_ENV_H_
#define _SOLAR_WIND_LISP_SCOPED_ENV_H_

#include <new>
#include <map>
#include "swl_matter_if.h"

namespace SolarWindLisp
{

class ScopedEnv
{
public:
    typedef std::map<std::string, IMatter *>::iterator ITER;
    typedef std::map<std::string, IMatter *>::const_iterator CONST_ITER;

    static ScopedEnv * create(ScopedEnv * ext = NULL)
    {
        return new (std::nothrow) ScopedEnv(ext);
    }

    bool add(const std::string& name, IMatter * value)
    {
        ITER it = _current.find(name);
        if (it != _current.end()) {
            return false;
        }

        _current.insert(std::pair<std::string, IMatter*>(name, value));
        return true;
    }

    bool lookup(const std::string& name, IMatter ** result)
    {
        ITER it = _current.find(name);
        if (it != _current.end()) {
            *result = it->second;
            return true;
        }

        if (!_external) {
            return false;
        }

        return _external->lookup(name, result);
    }

    // TODO: make ctor private
    ScopedEnv(ScopedEnv * ext = NULL) :
            _external(ext)
    {
    }

private:
    ScopedEnv * _external;
    std::map<std::string, IMatter *> _current;
};

} // namespace SolarWindLisp

#endif // _SOLAR_WIND_LISP_SCOPED_ENV_H_
