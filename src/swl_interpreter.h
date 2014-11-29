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
    InterpreterIF(IParser * parser = NULL, ScopedEnv * env = NULL,
            IMatterFactory * factory = NULL);
    virtual ~InterpreterIF();
    bool initialize();

    static const char * prompt(bool primary = true)
    {
        return primary ? "SolarWindLisp> " : "    ";
    }

    IParser * parser()
    {
        return _parser;
    }

    ScopedEnv * env()
    {
        return _env;
    }

    IMatterFactory * matter_factory()
    {
        return _factory;
    }

protected:
    typedef bool (*pred_func_t)(IMatter * expr);
    typedef bool (*eval_func_t)(IMatter * exrp, ScopedEnv * env,
            InterpreterIF * interpreter, IMatter ** result);

    static ScopedEnv * create_minimum_env();
    void _expand();

    /*
     * Description:
     *   Evaluate `expr' in `scope' using `interpreter', saves result in
     *   `result' if not NULL.
     * Return value:
     *   true if success, false on error.
     */
    static bool _eval(IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter, IMatter ** result = NULL);
    static bool _force_eval(IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter, IMatter ** result);

    /* primitive */
    static bool _is_prim(IMatter * expr)
    {
        if (expr->is_atom()) {
            const Expr * e = static_cast<const Expr *>(expr);
            if (e->is_numeric() || e->is_quoted_cstr()) {
                return true;
            }
        }

        return expr->is_prim_proc();
    }

    static bool _eval_prim(IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter, IMatter ** result);

    /* helper */
    static bool _is_special_form(IMatter * expr, const char * keyword);

    /* if */
    static bool _is_if(IMatter * expr)
    {
        return _is_special_form(expr, "if");
    }

    static bool _eval_if(IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter, IMatter ** result);

    /* definition */
    static bool _is_define(IMatter * expr)
    {
        return _is_special_form(expr, "define");
    }

    static bool _eval_define(IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter, IMatter ** result);

    /* name */
    static bool _is_name(IMatter * expr)
    {
        if (!expr->is_atom()) {
            return false;
        }

        const Expr * e = static_cast<const Expr *>(expr);
        if (!e->is_cstr() || e->is_quoted_cstr()) {
            return false;
        }

        return true;
    }

    static bool _eval_name(IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter UNUSED, IMatter ** result);

    /* time */
    static bool _is_time(IMatter * expr)
    {
        return _is_special_form(expr, "time");
    }

    static bool _eval_time(IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter, IMatter ** result);

    /* lambda & defn */
    static bool _is_lambda(IMatter * expr)
    {
        return _is_special_form(expr, "lambda");
    }

    static bool _eval_lambda(IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter, IMatter ** result);

    static bool _is_defn(IMatter * expr)
    {
        return _is_special_form(expr, "defn");
    }

    static bool _eval_defn(IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter UNUSED, IMatter ** result);

    /* cond, do, when */
    static bool _is_cond(IMatter * expr)
    {
        return _is_special_form(expr, "cond");
    }

    static bool _eval_cond(IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter, IMatter ** result);

    static bool _is_do(IMatter * expr)
    {
        return _is_special_form(expr, "do");
    }

    static bool _eval_do(IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter, IMatter ** result);

    static bool _is_when(IMatter * expr)
    {
        return _is_special_form(expr, "when");
    }

    static bool _eval_when(IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter, IMatter ** result);

    /* application */
    static bool _is_future(IMatter * expr)
    {
        return expr->is_future();
    }

    static bool _eval_future(IMatter * expr, ScopedEnv * scope UNUSED,
            InterpreterIF * interpreter UNUSED, IMatter ** result);

    static bool _is_app(IMatter * expr)
    {
        return expr->is_molecule();
    }

    static bool _eval_app(IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter, IMatter ** result);

    static bool _realize(IMatter * expr, IMatter ** result);

    static bool _apply(IMatter * proc_name, IMatter * proc_operands,
            InterpreterIF * interpreter, IMatter ** result);

public:
    bool execute(IMatter ** result, const char * str, ssize_t len = -1);
    bool execute_multi_expr(IMatter ** result, IMatter * expr);
    bool execute_expr(IMatter ** result, IMatter * expr);
    /* REPL */
    void cmd_help() const;
    static void banner();
    static void cmd_doc();
    static void cmd_names();
    static void _repl(InterpreterIF * interpreter,
            bool continue_on_error = true);
    void interactive(bool continue_on_error = true);
    static void repl(bool continue_on_error = true);

private:
    bool _initialized;
    IParser * _parser;
    ScopedEnv * _env;
    IMatterFactory * _factory;
};

class SimpleInterpreter: public InterpreterIF
{
};

} // namespace SolarWindLisp

#endif // _SOLAR_WIND_LISP_INTERPRETER_H_
