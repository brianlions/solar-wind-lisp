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
    static ScopedEnv * _create_minimum_env();

    typedef bool (*pred_func_t)(const IMatter * expr);
    typedef bool (*eval_func_t)(IMatter * exrp, ScopedEnv * env,
            InterpreterIF * interpreter, IMatter ** result);

#if 0
    void _expand()
    {
        const char * forms[] = { //
            "(define pi 3.141592653589793)",//
            "(define e  2.718281828459045)",//
        };

        for (size_t i = 0; i < array_size(forms); ++i) {
            // TODO: execute(forms[i])
        }
    }
#endif

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
        struct _pairs
        {
            pred_func_t pred;
            eval_func_t eval;
        } pairs[] = { //
                { _is_prim, _eval_prim }, //
#if 0
                {   _is_if, _eval_if}, //
                {   _is_define, _eval_define}, //
                {   _is_defn, _eval_defn}, //
                {   _is_cond, _eval_cond}, //
                {   _is_do, _eval_do}, //
                {   _is_when, _eval_when}, //
                {   _is_time, _eval_time}, //
                {   _is_name, _eval_name}, //
                {   _is_lambda, _eval_lambda}, //
                {   _is_app, _eval_app}, //
                {   _is_future, _eval_future}, //
#endif
            };

        for (size_t i = 0; i < array_size(pairs); ++i) {
            if (pairs[i].pred(expr)) {
#if 0
                if (pairs[i].eval(expr, scope, interpreter, result)) {
                    return true;
                }
#else
                PRETTY_MESSAGE(stderr,
                        "implementation not finished, result will be NULL!");
                return true;
#endif
            }
        }

        // TODO: report error
        return false;
    }

    static bool _force_eval(IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter, IMatter ** result = NULL)
    {
        IMatter * res = NULL;
        if (!_eval(expr, scope, interpreter, &res)) {
            return false;
        }

        if (_is_future(expr)) {
#if 0 // TODO impl
            *result = (static_cast<Future *>(res))->value();
#else
            PRETTY_MESSAGE(stderr, "not implemented!");
            exit(0);
            return false;
#endif
        }
        else {
            if (result) {
                *result = res;
            }
            else {
                PRETTY_MESSAGE(stderr, "discarded result");
            }

            return true;
        }
    }

    /* primitive */
    static bool _is_prim(const IMatter * expr)
    {
        if (expr->is_atom()) {
            return true;
        }
        else if (expr->is_prim_proc()) {
            return true;
        }
        else {
            return false;
        }
    }

    static bool _eval_prim(IMatter * expr, //
            ScopedEnv * scope UNUSED, InterpreterIF * interpreter UNUSED, //
            IMatter ** result)
    {
        *result = expr;
        return true;
    }

    static bool _is_number(const IMatter * expr); // probably not needed
    static bool _is_quoted(const IMatter * expr);
    static bool _is_prim_proc(const IMatter * expr); // XXX probably not needed
    // arithmetic
    static bool _prim_add(const IMatter * operands, IMatter &result);
    static bool _prim_sub(const IMatter * operands, IMatter &result);
    static bool _prim_mul(const IMatter * operands, IMatter &result);
    static bool _prim_div(const IMatter * operands, IMatter &result);
    static bool _prim_mod(const IMatter * operands, IMatter &result);
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
    static bool _check_operands(const IMatter *operands, size_t expected,
            const char * name);

    /* helper */
    static bool _is_special_form(const IMatter * expr, const char * keyword);

    /* if */
    static bool _is_if(const IMatter * expr);
    static bool _eval_if(const IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter, IMatter ** result);
    /* definition */
    static bool _is_define(const IMatter * expr);
    static bool _eval_define(const IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter, IMatter ** result);

    /* name */
    static bool _is_name(const IMatter * expr);
    static bool _eval_name(const IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter UNUSED, IMatter ** result);

    /* time */
    static bool _is_time(const IMatter * expr);
    static bool _eval_time(const IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter, IMatter ** result);

    /* lambda & defn */
    static bool _is_lambda(const IMatter * expr);
    static bool _eval_lambda(const IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter UNUSED, Proc * result); // XXX
    static bool _is_defn(const IMatter * expr);
    static bool _eval_defn(const IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter UNUSED, Proc * result); // XXX

    /* cond, do, when */
    static bool _is_cond(const IMatter * expr);
    static bool _eval_cond(const IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter, IMatter ** result);
    static bool _is_do(const IMatter * expr);
    static bool _eval_do(const IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter, IMatter ** result);
    static bool _is_when(const IMatter * expr);
    static bool _eval_when(const IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter, IMatter ** result);

    /* application */
    static bool _is_future(const IMatter * expr)
    {
        return expr->is_future();
    }

    static bool _eval_future(const IMatter * expr, ScopedEnv * scope UNUSED,
            InterpreterIF * interpreter UNUSED, IMatter ** result);
    static bool _is_app(const IMatter * expr);
    static bool _eval_app(const IMatter * expr, ScopedEnv * scope,
            InterpreterIF * interpreter, IMatter ** result);
    // FIXME: method signature TBD, first arg must not a const?
    static bool _realize(IMatter * expr, IMatter ** result);
    static bool _apply(IMatter * proc_name, IMatter * proc_operands,
            InterpreterIF * interpreter, IMatter ** result);

public:
    /* misc */
    // XXX make `result' the first arg?
    bool execute(const char * input, ssize_t input_length = 1, //
            IMatter ** result = NULL)
    {
        IExpr * forms = _parser->parse(input, input_length);
        if (!forms) {
            PRETTY_MESSAGE(stderr, "failed parsing input string!");
            return false;
        }

        if (!forms->is_molecule()) {
            PRETTY_MESSAGE(stderr,
                    "return value of parser is not a CompositeExpr!");
            return false;
        }

        return execute_multi_expr(forms, result);
    }

    bool execute_multi_expr(IMatter * expr, IMatter ** result)
    {
        if (!expr->is_molecule()) {
            PRETTY_MESSAGE(stderr, "expr is not CompositeExpr!");
            return false;
        }

        CompositeExpr * forms = static_cast<CompositeExpr *>(expr);
        //size_t n_forms = forms->size();
        if (!forms->rewind()) {
            PRETTY_MESSAGE(stderr, "rewind() failed!");
            return false;
        }

        while (forms->has_next()) {
            if (!execute_expr(forms->get_next(), result)) {
                PRETTY_MESSAGE(stderr, "failed executing form");
                return false;
            }
        }

        return true;
    }

    bool execute_expr(IMatter * expr, IMatter ** result)
    {
        if (!_force_eval(expr, this->_env, this, result)) {
            PRETTY_MESSAGE(stderr, "failed");
            return false;
        }

        return true;
    }

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
