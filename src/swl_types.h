/*
 * file name:           src/swl_types.h
 *
 * author:              Brian Yi ZHANG
 * email:               brianlions@gmail.com
 * date created:        Mon Nov 24 23:22:04 2014 CST
 */

#ifndef _SOLAR_WIND_LISP_TYPES_H_
#define _SOLAR_WIND_LISP_TYPES_H_

#include <boost/shared_ptr.hpp>

namespace SolarWindLisp
{

class IMatter;
class Expr;
class CompositeExpr;
class IPrimProc;
class Proc;
class Future;
class ScopedEnv;

typedef boost::shared_ptr<IMatter> MatterPtr;
typedef boost::shared_ptr<Expr> ExprPtr;
typedef boost::shared_ptr<CompositeExpr> CompositeExprPtr;
typedef boost::shared_ptr<Proc> ProcPtr;
typedef boost::shared_ptr<Future> FuturePtr;
typedef boost::shared_ptr<IPrimProc> PrimProcPtr;
typedef boost::shared_ptr<ScopedEnv> ScopedEnvPtr;

} // namespace SolarWindLisp

#endif // _SOLAR_WIND_LISP_TYPES_H_
