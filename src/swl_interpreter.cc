#include <stdio.h>
#include <stdlib.h>
#include "swl_interpreter.h"

namespace SolarWindLisp
{

InterpreterIF::InterpreterIF(IParser * parser, ScopedEnv * env,
        IMatterFactory * factory)
{
    _parser = parser;
    _env = env;
    _factory = factory;
    _initialized = _parser && _env && _factory;
}

InterpreterIF::~InterpreterIF()
{
    if (_parser) {
        delete _parser;
    }

    if (_env) {
        delete _env;
    }

    if (_factory) {
        delete _factory;
    }
}

bool InterpreterIF::initialize()
{
    if (_initialized) {
        return false;
    }

    if (!_parser) {
        if (!(_parser = new (std::nothrow) SimpleParser())) {
            return false;
        }
    }

    if (!_env) {
        if (!(_env = create_minimum_env())) {
            return false;
        }
    }

    if (!_factory) {
        if (!(_factory = new (std::nothrow) SimpleMatterFactory())) {
            return false;
        }
    }

    _initialized = true;
    return true;
}

ScopedEnv * InterpreterIF::create_minimum_env()
{
    ScopedEnv * ret = ScopedEnv::create();

    IPrimProc * pp = new (std::nothrow) PrimProcAdd();
    if (!pp) {
        PRETTY_MESSAGE(stderr, "failed create instance of %s", "PrimProcAdd");
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

void InterpreterIF::_expand()
{
    const char * forms[] = { //
            "(define pi 3.141592653589793)", //
            "(define e  2.718281828459045)", //
            "(define inc (lambda (x) (+ x 1)))", //
            "(define dec (lambda (x) (- x 1)))", //
            "(define max2 (lambda (a b) (if (> a b) a b)))", //
            "(define max3 (lambda (a b c) (max2 a (max2 b c))))", //
            "(define max4 (lambda (a b c d) (max2 a (max3 b c d))))", //
            "(defn min2 (a b) (if (< a b) a b))", //
            "(defn min3 (a b c) (min2 a (min2 b c)))", //
            "(defn min4 (a b c d) (min2 a (min3 b c d)))", //
            };

    for (size_t i = 0; i < array_size(forms); ++i) {
        if (!execute(NULL, forms[i], strlen(forms[i]))) {
            // XXX
        }
    }
}

bool InterpreterIF::_eval(IMatter * expr, ScopedEnv * scope,
        InterpreterIF * interpreter, IMatter ** result)
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
            return pairs[i].eval(expr, scope, interpreter, result);
        }
    }

    PRETTY_MESSAGE(stderr, "unknown expr type ...");
    return false;
}

bool InterpreterIF::_is_prim(IMatter * expr)
{
    if (expr->is_atom()) {
        const Expr * e = static_cast<const Expr *>(expr);
        if (e->is_numeric() || e->is_quoted_cstr()) {
            return true;
        }
    }

    return expr->is_prim_proc();
}

bool InterpreterIF::_is_special_form(IMatter * expr, const char * keyword)
{
    if (!expr->is_molecule()) {
        return false;
    }

    const CompositeExpr * ce = static_cast<const CompositeExpr *>(expr);
    if (!ce->size()) {
        return false;
    }

    const Expr * first = static_cast<const Expr *>(ce->get(0));
    if (!first->is_cstr()) {
        return false;
    }

    return strcmp(keyword, first->to_cstr()) == 0;
}

bool InterpreterIF::execute(IMatter ** result, const char * str, ssize_t len)
{
    IExpr * forms = _parser->parse(str, len);
    if (!forms) {
        PRETTY_MESSAGE(stderr, "failed parsing input string!");
        return false;
    }

    if (!forms->is_molecule()) {
        PRETTY_MESSAGE(stderr,
                "return value of parser is not a CompositeExpr!");
        return false;
    }

    return execute_multi_expr(result, forms);
}

bool InterpreterIF::execute_multi_expr(IMatter ** result, IMatter * expr)
{
    if (!expr->is_molecule()) {
        PRETTY_MESSAGE(stderr, "expr is not CompositeExpr!");
        return false;
    }

    CompositeExpr * forms = static_cast<CompositeExpr *>(expr);
    if (!forms->rewind()) {
        PRETTY_MESSAGE(stderr, "rewind() failed!");
        return false;
    }

    while (forms->has_next()) {
        if (!execute_expr(result, forms->get_next())) {
            PRETTY_MESSAGE(stderr, "failed executing form");
            return false;
        }
    }

    return true;
}

bool InterpreterIF::execute_expr(IMatter ** result, IMatter * expr)
{
    PRETTY_MESSAGE(stderr, "executing expr `%s' ...",
            expr->debug_string(false).c_str());
    if (!_force_eval(expr, this->_env, this, result)) {
        PRETTY_MESSAGE(stderr, "failed");
        return false;
    }

    return true;
}

void InterpreterIF::_repl(InterpreterIF * interpreter, bool continue_on_error)
{
    static const size_t default_buf_length = 4096;
    char * buffer = (char *) malloc(default_buf_length);
    if (!buffer) {
        PRETTY_MESSAGE(stderr, "memory allocation failed!");
        return;
    }

    IExpr * ie = NULL;
    CompositeExpr * ce = NULL;
    //Expr * e = NULL;

    size_t buffer_size = default_buf_length;
    ssize_t input_len = 0;
    while (true) {
        printf("%s", prompt());
        if ((input_len = getline(&buffer, &buffer_size, stdin)) == -1) {
            break;
        }

        ie = interpreter->parser()->parse(buffer, input_len);
        if (!ie) {
            continue;
        }

        if (!ie->is_molecule()) {
            PRETTY_MESSAGE(stderr, "this shoud not happen");
            break;
        }

        ce = static_cast<CompositeExpr *>(ie);
        ce->rewind();
        while (ce->has_next()) {
            IExpr * next = ce->get_next();
            PRETTY_MESSAGE(stderr, "executing expr `%s' ...",
                    next->debug_string(false).c_str());
            IMatter * a_result = NULL;
            if (!interpreter->execute_expr(&a_result, next)) {
                PRETTY_MESSAGE(stderr, "oops, failed!");
            }
            else if (a_result) {
                PRETTY_MESSAGE(stderr, "result: `%s'",
                        a_result->debug_string().c_str());
            }
            // some expr (e.g. define, defn) returns nothing
        }
    }

    free(buffer);
}

void InterpreterIF::interactive(bool continue_on_error)
{
    InterpreterIF::_repl(this, continue_on_error);
}

void InterpreterIF::repl(bool continue_on_error)
{
    InterpreterIF * interpreter = new SimpleInterpreter();
    if (!interpreter->initialize()) {
        PRETTY_MESSAGE(stderr, "initialization failed");
        delete interpreter;
    }

    InterpreterIF::_repl(interpreter, continue_on_error);
    delete interpreter;
}

} // namespace SolarWindLisp
