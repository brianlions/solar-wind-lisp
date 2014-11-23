#ifndef _SOLAR_WIND_LISP_INTERPRETER_H_
#define _SOLAR_WIND_LISP_INTERPRETER_H_

#include "gnu_attributes.h"
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

class InterpreterIF;

class InterpreterIF
{
public:
    InterpreterIF(IParser * parser = NULL, ScopedEnv * env = NULL,
            IMatterFactory * factory = NULL);
    virtual ~InterpreterIF();
    bool initialize();

    static const char * prompt(bool primary = true)
    {
        return primary ? "SolarWindLisp> " : "    ";
    }

protected:
    static ScopedEnv * _create_minimum_env()
    {
        PRETTY_MESSAGE(stderr, "not implemented");
        return NULL;
    }

    typedef bool (*pred_func_t)(const IMatter * expr);
    typedef bool (*eval_func_t)(const IMatter * exrp, ScopedEnv * env,
            InterpreterIF * interpreter, IMatter ** result);
    typedef bool (*prim_proc_t)(const CompositeExpr * operands,
            IMatter ** result);

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
#if 0
        // XXX this is the incorrect implementation :-(
        if (expr->is_atom()) {
            if (_is_quoted(expr)) {
                PRETTY_MESSAGE("not implemented");
            }
            *result = expr;
            return true;
        }

        prim_proc_t proc_func = 0;
        if ((proc_func = _is_prim_proc(expr))) {
            const CompositeExpr * ce = static_cast<const CompositeExpr *>(expr);
            return (*proc_func)(ce, result);
        }
#endif
        struct _pairs
        {
            pred_func_t pred;
            eval_func_t eval;
        } pairs[] = { //
                { _is_prim, _eval_prim }, //
                { _is_if, _eval_if }, //
                { _is_define, _eval_define }, //
                { _is_defn, _eval_defn }, //
                { _is_cond, _eval_cond }, //
                { _is_do, _eval_do }, //
                { _is_when, _eval_when }, //
                { _is_time, _eval_time }, //
                { _is_name, _eval_name }, //
                { _is_lambda, _eval_lambda }, //
                { _is_app, _eval_app }, //
                { _is_future, _eval_future }, //
                };

        for (size_t i = 0; i < array_size(pairs); ++i) {
            if (pairs[i].pred(expr)) {
                if (pairs[i].eval(expr, scope, interpreter, result)) {
                    return true;
                }
            }
        }

        PRETTY_MESSAGE(stderr, "unknown expr type ...");
        return false;
    }

    static bool _force_eval(IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter, IMatter ** result = NULL)
    {
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
    static bool _is_prim(const IMatter * expr);

    static bool _is_prim_proc(const IMatter * expr)
    {
        return expr->is_prim_proc();
    }

    static bool _eval_prim(const IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter, IMatter ** result)
    {
        if (expr->is_atom()) {
            const Expr * e = static_cast<const Expr *>(expr);
            if (e->is_numeric()) {
#if 0 // TODO set result
                *result = e;
                return true;
#else
                PRETTY_MESSAGE(stderr, "not implemented");
                return false;
#endif
            }

            if (_is_quoted(e)) {
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

    static bool _is_quoted(const Expr * expr)
    {
        PRETTY_MESSAGE(stderr, "not implemented");
        return false;
    }

    static Expr * _strip_quote_chars(const Expr * expr)
    {
        PRETTY_MESSAGE(stderr, "not implemented");
        return NULL;
    }

    struct prim_proc_table_elem_t
    {
        const char * name;
        prim_proc_t proc;
    };

#if 1
    // XXX need refactor
    static const struct prim_proc_table_elem_t _prim_proc_table[];

    static prim_proc_t _get_prim_proc(const IMatter * expr)
    {
        if (!expr->is_molecule()) {
            return NULL;
        }

        const CompositeExpr * ce = static_cast<const CompositeExpr *>(expr);
        if (!ce->get(0)->is_atom()) {
            return NULL;
        }

        const Expr * e = static_cast<const Expr *>(ce->get(0));
        const char * value = e->to_cstr();
        const prim_proc_table_elem_t * elem = _prim_proc_table;
        while (elem) {
            if (!strcmp(value, elem->name)) {
                return elem->proc;
            }
            elem++;
        }

        return NULL;
    }
#endif

    // arithmetic
    static bool _prim_add(const CompositeExpr * operands, IMatter ** result)
    {
        PRETTY_MESSAGE(stderr, "implement me, ASAP!");
        return false;
    }

    static bool _prim_sub(const CompositeExpr * operands, IMatter ** result)
    {
        PRETTY_MESSAGE(stderr, "implement me, ASAP!");
        return false;
    }

    static bool _prim_mul(const CompositeExpr * operands, IMatter ** result)
    {
        PRETTY_MESSAGE(stderr, "implement me, ASAP!");
        return false;
    }

    static bool _prim_div(const CompositeExpr * operands, IMatter ** result)
    {
        PRETTY_MESSAGE(stderr, "implement me, ASAP!");
        return false;
    }

    static bool _prim_mod(const CompositeExpr * operands, IMatter ** result)
    {
        PRETTY_MESSAGE(stderr, "implement me, ASAP!");
        return false;
    }

#if 1
    // XXX impl later
    // relative
    static bool _prim_eq(const IMatter * operands, IMatter &result);
    static bool _prim_ne(const IMatter * operands, IMatter &result);
    static bool _prim_lt(const IMatter * operands, IMatter &result);
    static bool _prim_le(const IMatter * operands, IMatter &result);
    static bool _prim_gt(const IMatter * operands, IMatter &result);
    static bool _prim_ge(const IMatter * operands, IMatter &result);
    // not
    static bool _prim_not(const IMatter * operands, IMatter &result);
    // bitwise ops
    static bool _prim_shift(const IMatter * operands, IMatter &result);
    static bool _prim_bitwise_and(const IMatter * operands, IMatter &result);
    static bool _prim_bitwise_or(const IMatter * operands, IMatter &result);
    static bool _prim_bitwise_xor(const IMatter * operands, IMatter &result);
    static bool _prim_bitwise_not(const IMatter * operands, IMatter &result);
    // sleep
    static bool _prim_sleep(const IMatter * operands, IMatter &result);
    // string ops
    static bool _prim_strlen(const IMatter * operands, IMatter &result);
    static bool _prim_format(const IMatter * operands, IMatter &result);
    static bool _prim_print(const IMatter * operands, IMatter &result);
    static bool _prim_println(const IMatter * operands, IMatter &result);
    static bool _prim_printf(const IMatter * operands, IMatter &result);
#endif

    static bool _check_operands(const IMatter *operands, size_t expected,
            const char * name)
    {
        PRETTY_MESSAGE(stderr, "not implemented");
        return false;
    }

    /* helper */
    static bool _is_special_form(const IMatter * expr, const char * keyword);

    /* if */
    static bool _is_if(const IMatter * expr)
    {
        return _is_special_form(expr, "if");
    }

    static bool _eval_if(const IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter, IMatter ** result)
    {
        PRETTY_MESSAGE(stderr, "not implemented");
        return false;
    }

    /* definition */
    static bool _is_define(const IMatter * expr)
    {
        return _is_special_form(expr, "define");
    }

    static bool _eval_define(const IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter, IMatter ** result)
    {
        PRETTY_MESSAGE(stderr, "not implemented");
        return false;
    }

    /* name */
    static bool _is_name(const IMatter * expr)
    {
        PRETTY_MESSAGE(stderr, "not implemented");
        return false;
    }

    static bool _eval_name(const IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter UNUSED, IMatter ** result)
    {
        PRETTY_MESSAGE(stderr, "not implemented");
        return false;
    }

    /* time */
    static bool _is_time(const IMatter * expr)
    {
        return _is_special_form(expr, "time");
    }

    static bool _eval_time(const IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter, IMatter ** result)
    {
        PRETTY_MESSAGE(stderr, "not implemented");
        return false;
    }

    /* lambda & defn */
    static bool _is_lambda(const IMatter * expr)
    {
        return _is_special_form(expr, "lambda");
    }

    static bool _eval_lambda(const IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter UNUSED, IMatter ** result)
    {
        PRETTY_MESSAGE(stderr, "not implemented");
        return false;
    }

    static bool _is_defn(const IMatter * expr)
    {
        return _is_special_form(expr, "defn");
    }

    static bool _eval_defn(const IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter UNUSED, IMatter ** result)
    {
        PRETTY_MESSAGE(stderr, "not implemented");
        return false;
    }

    /* cond, do, when */
    static bool _is_cond(const IMatter * expr)
    {
        return _is_special_form(expr, "cond");
    }

    static bool _eval_cond(const IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter, IMatter ** result)
    {
        PRETTY_MESSAGE(stderr, "not implemented");
        return false;
    }

    static bool _is_do(const IMatter * expr)
    {
        return _is_special_form(expr, "do");
    }

    static bool _eval_do(const IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter, IMatter ** result)
    {
        PRETTY_MESSAGE(stderr, "not implemented");
        return false;
    }

    static bool _is_when(const IMatter * expr)
    {
        return _is_special_form(expr, "when");
    }

    static bool _eval_when(const IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter, IMatter ** result)
    {
        PRETTY_MESSAGE(stderr, "not implemented");
        return false;
    }

    /* application */
    static bool _is_future(const IMatter * expr)
    {
        return expr->is_future();
    }

    static bool _eval_future(const IMatter * expr, ScopedEnv * scope UNUSED,
            InterpreterIF * interpreter UNUSED, IMatter ** result)
    {
        PRETTY_MESSAGE(stderr, "not implemented");
        return false;
    }

    static bool _is_app(const IMatter * expr)
    {
        // XXX: an app is a list (CompositeExpr/molecule).
        return expr->is_molecule();
    }

    static bool _eval_app(const IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter, IMatter ** result)
    {
        PRETTY_MESSAGE(stderr, "not implemented");
        return false;
    }

    // FIXME: method signature TBD, first arg must not a const?
    static bool _realize(IMatter * expr, IMatter ** result)
    {
        if (expr->is_future()) {
#if 0
            // XXX impl following:
            // *result = static_cast<Future *>(expr)->value();
            // return true;
#else
            PRETTY_MESSAGE(stderr, "not implemented");
            return false;
#endif
        }
        else {
#if 0
            // XXX impl following:
            // *result = expr;
            return true;
#else
            PRETTY_MESSAGE(stderr, "not implemented");
            return false;
#endif
        }
    }

    static bool _apply(IMatter * proc_name, IMatter * proc_operands,
            InterpreterIF * interpreter, IMatter ** result)
    {
        PRETTY_MESSAGE(stderr, "not implemented");
        return false;
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
