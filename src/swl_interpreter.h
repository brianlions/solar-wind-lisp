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
    static ScopedEnv * create_minimum_env()
    {
        ScopedEnv * ret = ScopedEnv::create();

        IPrimProc * pp = new (std::nothrow) PrimProcAdd();
        if (!pp) {
            PRETTY_MESSAGE(stderr, "failed create instance of %s",
                    "PrimProcAdd");
            delete ret;
            return NULL;
        }

        if (!ret->add(pp->name(), pp)) {
            delete pp;
            delete ret;
            return NULL;
        }

        return ret;
    }

    typedef bool (*pred_func_t)(IMatter * expr);
    typedef bool (*eval_func_t)(IMatter * exrp, ScopedEnv * env,
            InterpreterIF * interpreter, IMatter ** result);

    void _expand();

    /*
     * Description:
     *   Evaluate `expr' in `scope' using `interpreter', saves result in
     *   `result' if not NULL.
     * Return value:
     *   true if success, false on error.
     */
    static bool _eval(IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter, IMatter ** result = NULL)
    {
        PRETTY_MESSAGE(stderr, "executing `%s' ...",
                expr->debug_string(false).c_str());
        struct _pairs
        {
            pred_func_t pred;
            eval_func_t eval;
            const char * name;
        } pairs[] = { //
                { _is_prim, _eval_prim, "prim" }, //
                { _is_if, _eval_if, "if" }, //
                { _is_define, _eval_define, "define" }, //
                { _is_defn, _eval_defn, "defn" }, //
                { _is_cond, _eval_cond, "cond" }, //
                { _is_do, _eval_do, "do" }, //
                { _is_when, _eval_when, "when" }, //
                { _is_time, _eval_time, "time" }, //
                { _is_name, _eval_name, "name" }, //
                { _is_lambda, _eval_lambda, "lambda" }, //
                { _is_app, _eval_app, "app" }, //
                { _is_future, _eval_future, "future" }, //
                };

        for (size_t i = 0; i < array_size(pairs); ++i) {
            PRETTY_MESSAGE(stderr, "test pred `%s' ...", pairs[i].name);
            if (pairs[i].pred(expr)) {
                PRETTY_MESSAGE(stderr, "eval pred `%s' ...", pairs[i].name);
                if (pairs[i].eval(expr, scope, interpreter, result)) {
                    return true;
                }
            }
        }

        PRETTY_MESSAGE(stderr, "unknown expr type ...");
        return false;
    }

    static bool _force_eval(IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter, IMatter ** result)
    {
        PRETTY_MESSAGE(stderr, "executing `%s' ...",
                expr->debug_string(false).c_str());
        IMatter * res = NULL;
        if (!_eval(expr, scope, interpreter, &res)) {
            PRETTY_MESSAGE(stderr, "oops ...");
            return false;
        }

        if (_is_future(expr)) {
            // TODO impl
            // *result = (static_cast<Future *>(res))->value();
            PRETTY_MESSAGE(stderr, "not implemented");
            return false;
        }

        if (result) {
            *result = res;
        }
        else {
            PRETTY_MESSAGE(stderr, "result discarded");
        }

        return true;
    }

    /* primitive */
    static bool _is_prim(IMatter * expr);

    // TODO move into _is_prim()
    static bool _is_prim_proc(IMatter * expr)
    {
        return expr->is_prim_proc();
    }

    static bool _eval_prim(IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter, IMatter ** result)
    {
        if (expr->is_atom()) {
            const Expr * e = static_cast<const Expr *>(expr);
            if (e->is_numeric()) {
                *result = expr;
                return true;
            }

            if (e->is_quoted_cstr()) {
#if 0 // TODO
                IMatter * res = _strip_quote_chars(e);
                *result = res;
                return true;
#else
                PRETTY_MESSAGE(stderr, "not implemented");
                return false;
#endif
            }
        }

#if 0 // TODO set result
        *result = expr;
        return true;
#else
        PRETTY_MESSAGE(stderr, "not implemented");
        return false;
#endif
    }

    /* helper */
    static bool _is_special_form(IMatter * expr, const char * keyword);

    /* if */
    static bool _is_if(IMatter * expr)
    {
        return _is_special_form(expr, "if");
    }

    static bool _eval_if(IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter, IMatter ** result)
    {
        PRETTY_MESSAGE(stderr, "not implemented");
        return false;
    }

    /* definition */
    static bool _is_define(IMatter * expr)
    {
        return _is_special_form(expr, "define");
    }

    static bool _eval_define(IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter, IMatter ** result)
    {
        PRETTY_MESSAGE(stderr, "not implemented");
        return false;
    }

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
            InterpreterIF * interpreter UNUSED, IMatter ** result)
    {
        PRETTY_MESSAGE(stderr, "execute: `%s' ...", expr->debug_string(false).c_str());
        if (expr->is_atom()) {
            Expr * e = static_cast<Expr *>(expr);
            if (e->is_cstr() && scope->lookup(e->to_cstr(), result)) {
                return true;
            }
        }
        return false;
    }

    /* time */
    static bool _is_time(IMatter * expr)
    {
        return _is_special_form(expr, "time");
    }

    static bool _eval_time(IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter, IMatter ** result)
    {
        PRETTY_MESSAGE(stderr, "not implemented");
        return false;
    }

    /* lambda & defn */
    static bool _is_lambda(IMatter * expr)
    {
        return _is_special_form(expr, "lambda");
    }

    static bool _eval_lambda(IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter UNUSED, IMatter ** result)
    {
        PRETTY_MESSAGE(stderr, "not implemented");
        return false;
    }

    static bool _is_defn(IMatter * expr)
    {
        return _is_special_form(expr, "defn");
    }

    static bool _eval_defn(IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter UNUSED, IMatter ** result)
    {
        PRETTY_MESSAGE(stderr, "not implemented");
        return false;
    }

    /* cond, do, when */
    static bool _is_cond(IMatter * expr)
    {
        return _is_special_form(expr, "cond");
    }

    static bool _eval_cond(IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter, IMatter ** result)
    {
        PRETTY_MESSAGE(stderr, "not implemented");
        return false;
    }

    static bool _is_do(IMatter * expr)
    {
        return _is_special_form(expr, "do");
    }

    static bool _eval_do(IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter, IMatter ** result)
    {
        PRETTY_MESSAGE(stderr, "not implemented");
        return false;
    }

    static bool _is_when(IMatter * expr)
    {
        return _is_special_form(expr, "when");
    }

    static bool _eval_when(IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter, IMatter ** result)
    {
        PRETTY_MESSAGE(stderr, "not implemented");
        return false;
    }

    /* application */
    static bool _is_future(IMatter * expr)
    {
        return expr->is_future();
    }

    static bool _eval_future(IMatter * expr, ScopedEnv * scope UNUSED,
            InterpreterIF * interpreter UNUSED, IMatter ** result)
    {
        PRETTY_MESSAGE(stderr, "not implemented");
        return false;
    }

    static bool _is_app(IMatter * expr)
    {
        return expr->is_molecule();
    }

    static bool _eval_app(IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter, IMatter ** result)
    {
        PRETTY_MESSAGE(stderr, "executing `%s' ...",
                expr->debug_string(false).c_str());
        CompositeExpr * ce = static_cast<CompositeExpr *>(expr);
        if (!ce->rewind() || !ce->has_next()) {
            return false;
        }

        IExpr * first = ce->get_next();
        CompositeExpr * args = interpreter->matter_factory()->create_molecule();
        if (!args) {
            return false;
        }

        IExpr * ie = NULL;
        while (ce->has_next()) {
            ie = ce->get_next();
            Future * f = interpreter->matter_factory()->create_future(ie, scope,
                    interpreter);
            args->append_expr(f);
        }

        IMatter * p = NULL;
        return _force_eval(first, scope, interpreter, &p)
            ?  _apply(p, args, interpreter, result)
            : false;
    }

    // FIXME: method signature TBD, first arg must not a const?
    static bool _realize(IMatter * expr, IMatter ** result)
    {
        if (expr->is_future()) {
            Future * future = static_cast<Future *>(expr);
            if (future->value(result)) {
                return true;
            }
            else {
                return false;
            }
        }
        else {
            *result = expr;
            return true;
        }
    }

    static bool _apply(IMatter * proc_name, IMatter * proc_operands,
            InterpreterIF * interpreter, IMatter ** result)
    {
        PRETTY_MESSAGE(stderr, "proc: `%s', operands: `%s'",
                proc_name->debug_string(false).c_str(),
                proc_operands->debug_string(false).c_str());
        if (proc_name->is_prim_proc()) {
            IPrimProc * p = static_cast<IPrimProc *>(proc_name);
            // FIXME: _realize operands
            CompositeExpr * ce = interpreter->matter_factory()->create_molecule();
            if (!ce) {
                return false;
            }

            CompositeExpr * operands = static_cast<CompositeExpr *>(proc_operands);
            operands->rewind();
            while (operands->has_next()) {
                IMatter * res = NULL;
                if (!_realize(operands->get_next(), &res)) {
                    return false;
                }
                ce->append_expr(static_cast<IExpr *>(res));
            }
            return p->run(ce, result, interpreter->matter_factory());
        }
        else if (proc_name->is_proc()) {
            PRETTY_MESSAGE(stderr, "branch not implemented");
            return false;
        }
        else {
            return false;
        }
    }

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
