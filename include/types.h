/*
 * file name:           include/types.h
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

class MatterIF;
class Atom;
class CompositeExpr;
class PrimProcIF;
class Proc;
class Future;
class ScopedEnv;

typedef boost::shared_ptr<MatterIF> MatterPtr;
typedef boost::shared_ptr<Atom> AtomPtr;
typedef boost::shared_ptr<CompositeExpr> CompositeExprPtr;
typedef boost::shared_ptr<Proc> ProcPtr;
typedef boost::shared_ptr<Future> FuturePtr;
typedef boost::shared_ptr<PrimProcIF> PrimProcPtr;
typedef boost::shared_ptr<ScopedEnv> ScopedEnvPtr;
typedef PrimProcPtr (*prim_proc_ptr_creator_func_t)();

} // namespace SolarWindLisp

#endif // _SOLAR_WIND_LISP_TYPES_H_
