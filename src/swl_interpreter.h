/*
 * file name:           src/swl_interpreter.h
 *
 * author:              Brian Yi ZHANG
 * email:               brianlions@gmail.com
 * date created:        Sat Nov 22 23:15:18 2014 CST
 */

#ifndef _SOLAR_WIND_LISP_INTERPRETER_H_
#define _SOLAR_WIND_LISP_INTERPRETER_H_

#include "gnu_attributes.h"
#include "swl_types.h"
#include "swl_expr.h"
#include "swl_parser.h"
#include "swl_scoped_env.h"
#include "swl_proc.h"
#include "swl_future.h"
#include "swl_utils.h"
#include "swl_matter_factory_if.h"
#include "pretty_message.h"

namespace SolarWindLisp
{

class InterpreterIF
{
    friend class Future;
public:
    InterpreterIF(IParser * parser = NULL, ScopedEnvPtr env = NULL,
            IMatterFactory * factory = NULL);
    virtual ~InterpreterIF();
    bool initialize();

    static const char * prompt(bool primary = true)
    {
        return primary
            ? "SolarWindLisp> "
            : "               ";
    }

    IParser * parser()
    {
        return _parser;
    }

    ScopedEnvPtr env()
    {
        return _env;
    }

    IMatterFactory * factory()
    {
        return _factory;
    }

protected:
    typedef bool (*pred_func_t)(const MatterPtr &expr);
    typedef bool (*eval_func_t)(const MatterPtr &exrp, ScopedEnvPtr &env,
            InterpreterIF * interpreter, MatterPtr &result);

    ScopedEnvPtr create_minimum_env();
    void _expand();

    /*
     * Description:
     *   Evaluate `expr' in `scope' using `interpreter', saves result in
     *   `result' if not NULL.
     * Return value:
     *   true if success, false on error.
     */
    static bool _eval(const MatterPtr &expr, ScopedEnvPtr &scope,
            InterpreterIF * interpreter, MatterPtr &result);
    static bool _force_eval(const MatterPtr &expr, ScopedEnvPtr &scope,
            InterpreterIF * interpreter, MatterPtr &result);

    // primitive
    static bool _is_prim(const MatterPtr &expr)
    {
        if (expr->is_atom()) {
            const Expr * e = static_cast<const Expr *>(expr.get());
            if (e->is_numeric() || e->is_quoted_cstr()) {
                return true;
            }
        }

        return expr->is_prim_proc();
    }

    static bool _eval_prim(const MatterPtr &expr, ScopedEnvPtr &scope,
            InterpreterIF * interpreter, MatterPtr &result);

    // helper
    static bool _is_special_form(const MatterPtr &expr, const char * keyword);

    // if
    static bool _is_if(const MatterPtr &expr)
    {
        return _is_special_form(expr, "if");
    }

    static bool _eval_if(const MatterPtr &expr, ScopedEnvPtr &scope,
            InterpreterIF * interpreter, MatterPtr &result);

    // definition
    static bool _is_define(const MatterPtr &expr)
    {
        return _is_special_form(expr, "define");
    }

    static bool _eval_define(const MatterPtr &expr, ScopedEnvPtr &scope,
            InterpreterIF * interpreter, MatterPtr &result);

    // name
    static bool _is_name(const MatterPtr &expr)
    {
        if (!expr->is_atom()) {
            return false;
        }

        const Expr * e = static_cast<const Expr *>(expr.get());
        return e->is_cstr() && !e->is_quoted_cstr();
    }

    static bool _eval_name(const MatterPtr &expr, ScopedEnvPtr &scope,
            InterpreterIF * interpreter UNUSED, MatterPtr &result);

    // time
    static bool _is_time(const MatterPtr &expr)
    {
        return _is_special_form(expr, "time");
    }

    static bool _eval_time(const MatterPtr &expr, ScopedEnvPtr &scope,
            InterpreterIF * interpreter, MatterPtr &result);

    // lambda & defn
    static bool _is_lambda(const MatterPtr &expr)
    {
        return _is_special_form(expr, "lambda");
    }

    static bool _eval_lambda(const MatterPtr &expr, ScopedEnvPtr &scope,
            InterpreterIF * interpreter, MatterPtr &result);

    static bool _is_defn(const MatterPtr &expr)
    {
        return _is_special_form(expr, "defn");
    }

    static bool _eval_defn(const MatterPtr &expr, ScopedEnvPtr &scope,
            InterpreterIF * interpreter UNUSED, MatterPtr &result);

    // cond, do, when
    static bool _is_cond(const MatterPtr &expr)
    {
        return _is_special_form(expr, "cond");
    }

    static bool _eval_cond(const MatterPtr &expr, ScopedEnvPtr &scope,
            InterpreterIF * interpreter, MatterPtr &result);

    static bool _is_do(const MatterPtr &expr)
    {
        return _is_special_form(expr, "do");
    }

    static bool _eval_do(const MatterPtr &expr, ScopedEnvPtr &scope,
            InterpreterIF * interpreter, MatterPtr &result);

    static bool _is_when(const MatterPtr &expr)
    {
        return _is_special_form(expr, "when");
    }

    static bool _eval_when(const MatterPtr &expr, ScopedEnvPtr &scope,
            InterpreterIF * interpreter, MatterPtr &result);

    // application
    static bool _is_future(const MatterPtr &expr)
    {
        return expr->is_future();
    }

    static bool _eval_future(const MatterPtr &expr, ScopedEnvPtr &scope UNUSED,
            InterpreterIF * interpreter UNUSED, MatterPtr &result);

    static bool _is_app(const MatterPtr &expr)
    {
        return expr->is_molecule();
    }

    static bool _eval_app(const MatterPtr &expr, ScopedEnvPtr &scope,
            InterpreterIF * interpreter, MatterPtr &result);

    static bool _realize(const MatterPtr &expr, MatterPtr &result);

    static bool _apply(const MatterPtr &proc_name,
            const MatterPtr &proc_operands,
            InterpreterIF * interpreter, MatterPtr &result);

public:
    bool execute(MatterPtr &result, const char * str, ssize_t len = -1);
    bool execute_multi_expr(MatterPtr &result, const MatterPtr &expr);
    bool execute_expr(MatterPtr &result, const MatterPtr &expr);
    // REPL
    void cmd_help() const;
    static void banner();
    static void cmd_doc();
    static void cmd_names();
    static void _repl(InterpreterIF * interpreter, const char * filename = NULL);
    void interactive(const char * filename = NULL);
    static void repl(const char * filename = NULL);

private:
    bool _initialized;
    IParser * _parser;
    ScopedEnvPtr _env;
    IMatterFactory * _factory;
};

class SimpleInterpreter: public InterpreterIF
{
};

} // namespace SolarWindLisp

#endif // _SOLAR_WIND_LISP_INTERPRETER_H_
