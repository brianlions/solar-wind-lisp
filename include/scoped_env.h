/*
 * file name:           include/scoped_env.h
 *
 * author:              Brian Yi ZHANG
 * email:               brianlions@gmail.com
 * date created:        Sat Nov 22 23:15:18 2014 CST
 */

#ifndef _SOLAR_WIND_LISP_SCOPED_ENV_H_
#define _SOLAR_WIND_LISP_SCOPED_ENV_H_

#include <new>
#include <map>
#include "matter.h"

namespace SolarWindLisp
{

class ScopedEnv
{
    typedef std::map<std::string, MatterPtr>::iterator ITER;
    typedef std::map<std::string, MatterPtr>::const_iterator CONST_ITER;

public:
    static ScopedEnvPtr create(ScopedEnvPtr ext = NULL)
    {
        return ScopedEnvPtr(new (std::nothrow) ScopedEnv(ext));
    }

    bool add(const std::string& name, const MatterPtr &value)
    {
        ITER it = _current.find(name);
        if (it != _current.end()) {
            return false;
        }

        _current.insert(std::pair<std::string, MatterPtr>(name, value));
        return true;
    }

    bool lookup(const std::string& name, MatterPtr &result)
    {
        ITER it = _current.find(name);
        if (it != _current.end()) {
            result = it->second;
            return true;
        }

        return _external.get() ? _external->lookup(name, result) : false;
    }

    void clear()
    {
        // XXX remove circular reference
        for (ITER it = _current.begin(); it != _current.end(); ++it) {
            it->second.reset();
        }
        _external.reset();
    }

    virtual ~ScopedEnv()
    {
        clear();
    }

private:
    ScopedEnv(ScopedEnvPtr ext = NULL) :
            _external(ext)
    {
    }

    ScopedEnvPtr _external;
    std::map<std::string, MatterPtr> _current;
};

} // namespace SolarWindLisp

#endif // _SOLAR_WIND_LISP_SCOPED_ENV_H_
