#ifndef _SOLAR_WIND_LISP_COMPOSITE_H_
#define _SOLAR_WIND_LISP_COMPOSITE_H_

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <errno.h>

#include <limits>
#include <string>

#include "swl_matter_if.h"

namespace SolarWindLisp
{

class IExpr: public IMatter
{
    friend class Expr;
    friend class CompositeExpr;

public:

    IExpr() :
            _prev(NULL), _next(NULL)
    {
    }

    virtual ~IExpr()
    {
    }

    virtual size_t size() const
    {
        return 0;
    }

protected:

    IExpr * prev()
    {
        return _prev;
    }

    const IExpr * prev() const
    {
        return _prev;
    }

    const IExpr * next() const
    {
        return _next;
    }

    IExpr * next()
    {
        return _next;
    }

    void set_prev(IExpr * p)
    {
        _prev = p;
    }

    void set_next(IExpr * p)
    {
        _next = p;
    }

    void set_prev_and_next(IExpr * p, IExpr * n)
    {
        _prev = p;
        _next = n;
    }

    IExpr * _prev;
    IExpr * _next;
};

} // namespace SolarWindLisp

#endif // _SOLAR_WIND_LISP_COMPOSITE_H_
