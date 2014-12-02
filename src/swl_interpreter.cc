#include <stdio.h>
#include <stdlib.h>
#include "swl_interpreter.h"

namespace SolarWindLisp
{

InterpreterIF::InterpreterIF(IParser * parser, ScopedEnvPtr env,
        IMatterFactory * factory)
{
    _parser = parser;
    _env = env;
    _factory = factory;
    _initialized = _parser && _env && _factory;
}

InterpreterIF::~InterpreterIF()
{
    if (_env) {
        // XXX remove circular reference
        _env->clear();
    }

    if (_parser) {
        delete _parser;
    }

    if (_factory) {
        delete _factory;
    }
}

bool InterpreterIF::initialize()
{
    if (_initialized) {
        return true;
    }

    if (!_parser) {
        if (!(_parser = new (std::nothrow) SimpleParser())) {
            return false;
        }
    }

    if (!_factory) {
        if (!(_factory = new (std::nothrow) SimpleMatterFactory())) {
            return false;
        }
    }

    if (!_env) {
        if (!(_env = create_minimum_env())) {
            return false;
        }
    }

    _initialized = true;
    return true;
}

ScopedEnvPtr InterpreterIF::create_minimum_env()
{
    ScopedEnvPtr ret = _factory->create_env();
    if (!ret.get()) {
        return NULL;
    }

    struct PrimProcCreator {
        const char * name;
        PrimProcPtr (*func_ptr)();
    } items[] = {
        { "+", PrimProcAdd::create },
        { "-", PrimProcSub::create },
        { "*", PrimProcMul::create },
        { "/", PrimProcDiv::create },
    };

    for (size_t i = 0; i < array_size(items); ++i) {
        PrimProcPtr pp = items[i].func_ptr();
        if (!pp) {
            /*
             * FIXME:
             *   1. delete elements already added into `ret', or else mem leak!
             *   2. or maybe do that in ScopedEnv's dtor?
             */
            PRETTY_MESSAGE(stderr, "failed create instance of %s",
                    items[i].name);
            return NULL;
        }
        if (!ret->add(pp->name(), pp)) {
            return NULL;
        }
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

    MatterPtr result;
    for (size_t i = 0; i < array_size(forms); ++i) {
        if (!execute(result, forms[i], strlen(forms[i]))) {
        }
    }
}

bool InterpreterIF::_eval(const MatterPtr &expr, ScopedEnvPtr &scope,
        InterpreterIF * interpreter, MatterPtr &result)
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

bool InterpreterIF::_force_eval(const MatterPtr &expr, ScopedEnvPtr &scope,
        InterpreterIF * interpreter, MatterPtr &result)
{
    PRETTY_MESSAGE(stderr, "executing `%s' ...",
            expr->debug_string(false).c_str());
    MatterPtr res = NULL;
    if (!_eval(expr, scope, interpreter, res)) {
        PRETTY_MESSAGE(stderr, "oops ...");
        return false;
    }

#if 0
    // XXX is it always ok to return immediately if `result' is null?
    if (!result) {
        // caller does not care about the result
        return true;
    }
#endif

    if (res && _is_future(res)) {
        // beware of those forms returns nothing (e.g. define and defn)
        return static_cast<Future *>(res.get())->value(result);
    }
    else {
        // 1. res is NULL, or 2. res is not a future
        result = res;
        return true;
    }
}

bool InterpreterIF::_eval_prim(const MatterPtr &expr, ScopedEnvPtr &scope,
        InterpreterIF * interpreter, MatterPtr &result)
{
    result = expr;
    return true;
}

bool InterpreterIF::_is_special_form(const MatterPtr &expr, const char * keyword)
{
    if (!expr->is_molecule()) {
        return false;
    }

    const CompositeExpr * ce = static_cast<const CompositeExpr *>(expr.get());
    if (!ce->size()) {
        return false;
    }

    const Expr * first = static_cast<const Expr *>(ce->get(0).get());
    if (!first->is_cstr()) {
        return false;
    }

    return strcmp(keyword, first->to_cstr()) == 0;
}

bool InterpreterIF::_eval_if(const MatterPtr &expr, ScopedEnvPtr &scope,
        InterpreterIF * interpreter, MatterPtr &result)
{
    CompositeExpr * ce = static_cast<CompositeExpr *>(expr.get());
    MatterPtr pred = ce->get(1);
    MatterPtr cons = ce->get(2);
    MatterPtr alt = ce->get(3);
    MatterPtr res = NULL;
    if (!pred || !_force_eval(pred, scope, interpreter, res)) {
        return false;
    }

    MatterPtr final_form = alt;
    if (res) {
        if (res->is_atom() && static_cast<Expr *>(res.get())->not_empty()) {
            final_form = cons;
        }
        else if (res->is_molecule()
                && static_cast<CompositeExpr *>(res.get())->size()) {
            final_form = cons;
        }
        else if (res->is_prim_proc()) {
            final_form = cons;
        }
    }

    // result will be set by the following _eval()
    result = NULL;

    // Notice:
    //   if final_form == null, we do not need to evaluate anything, just
    //   to return `true' to the caller.
    return final_form
            ? _eval(final_form, scope, interpreter, result)
            : true;
}

bool InterpreterIF::_eval_define(const MatterPtr &expr, ScopedEnvPtr &scope,
        InterpreterIF * interpreter, MatterPtr &result)
{
    CompositeExpr * ce = static_cast<CompositeExpr *>(expr.get());
    if (ce->size() != 3) {
        return false;
    }

    MatterPtr pos1 = ce->get(1);
    MatterPtr pos2 = ce->get(2);
    if (!pos1->is_atom()) {
        return false;
    }

    Expr * name = static_cast<Expr *>(pos1.get());
    if (!name->is_cstr() || name->is_quoted_cstr()) {
        return false;
    }

    MatterPtr value = NULL;
    // there is no return value from a define
    result = NULL;
    return _eval(pos2, scope, interpreter, value)
            && scope->add(name->to_cstr(), value);
}

bool InterpreterIF::_eval_name(const MatterPtr &expr, ScopedEnvPtr &scope,
        InterpreterIF * interpreter UNUSED, MatterPtr &result)
{
    PRETTY_MESSAGE(stderr, "execute: `%s' ...",
            expr->debug_string(false).c_str());
    if (expr->is_atom()) {
        Expr * e = static_cast<Expr *>(expr.get());
        if (e->is_cstr() && scope->lookup(e->to_cstr(), result)) {
            return true;
        }
    }
    return false;
}

bool InterpreterIF::_eval_time(const MatterPtr &expr, ScopedEnvPtr &scope,
        InterpreterIF * interpreter, MatterPtr &result)
{
    PRETTY_MESSAGE(stderr, "not implemented");
    return false;
}

bool InterpreterIF::_eval_lambda(const MatterPtr &expr, ScopedEnvPtr &scope,
        InterpreterIF * interpreter, MatterPtr &result)
{
    CompositeExpr * ce = static_cast<CompositeExpr *>(expr.get());
    MatterPtr p1 = ce->get(1);
    MatterPtr p2 = ce->get(2);
    if (!p1 || !p1->is_molecule() || !p2) {
        return false;
    }

    ProcPtr p = interpreter->factory()->create_proc(p1, p2, scope);
    if (!p) {
        return false;
    }

    result = p;
    return true;
}

bool InterpreterIF::_eval_defn(const MatterPtr &expr, ScopedEnvPtr &scope,
        InterpreterIF * interpreter UNUSED, MatterPtr &result)
{
    PRETTY_MESSAGE(stderr, "not implemented");
    return false;
}

bool InterpreterIF::_eval_cond(const MatterPtr &expr, ScopedEnvPtr &scope,
        InterpreterIF * interpreter, MatterPtr &result)
{
    PRETTY_MESSAGE(stderr, "not implemented");
    return false;
}

bool InterpreterIF::_eval_do(const MatterPtr &expr, ScopedEnvPtr &scope,
        InterpreterIF * interpreter, MatterPtr &result)
{
    PRETTY_MESSAGE(stderr, "not implemented");
    return false;
}

bool InterpreterIF::_eval_when(const MatterPtr &expr, ScopedEnvPtr &scope,
        InterpreterIF * interpreter, MatterPtr &result)
{
    PRETTY_MESSAGE(stderr, "not implemented");
    return false;
}

bool InterpreterIF::_eval_future(const MatterPtr &expr, ScopedEnvPtr &scope UNUSED,
        InterpreterIF * interpreter UNUSED, MatterPtr &result)
{
    result = expr;
    return true;
}

bool InterpreterIF::_eval_app(const MatterPtr &expr, ScopedEnvPtr &scope,
        InterpreterIF * interpreter, MatterPtr &result)
{
    PRETTY_MESSAGE(stderr, "executing `%s' ...",
            expr->debug_string(false).c_str());
    CompositeExpr * ce = static_cast<CompositeExpr *>(expr.get());
    if (!ce->rewind() || !ce->has_next()) {
        return false;
    }

    MatterPtr first = ce->get_next();
    CompositeExprPtr args = interpreter->factory()->create_molecule();
    if (!args) {
        return false;
    }

    MatterPtr ie = NULL;
    while (ce->has_next()) {
        ie = ce->get_next();
        FuturePtr f = interpreter->factory()->create_future(ie, scope,
                interpreter);
        if (!f) {
            PRETTY_MESSAGE(stderr, "failed creating Future object");
            return false;
        }
        args->append_expr(f);
    }

    MatterPtr p = NULL;
    return _force_eval(first, scope, interpreter, p)
        ?  _apply(p, args, interpreter, result)
        : false;
}

bool InterpreterIF::_realize(const MatterPtr &expr, MatterPtr &result)
{
    if (expr->is_future()) {
        Future * future = static_cast<Future *>(expr.get());
        return future->value(result);
    }
    else {
        result = expr;
        return true;
    }
}

bool InterpreterIF::_apply(const MatterPtr &proc_name, const MatterPtr &proc_operands,
        InterpreterIF * interpreter, MatterPtr &result)
{
    PRETTY_MESSAGE(stderr, "proc: `%s', operands: `%s'",
            proc_name->debug_string(false).c_str(),
            proc_operands->debug_string(false).c_str());
    if (proc_name->is_prim_proc()) {
        IPrimProc * p = static_cast<IPrimProc *>(proc_name.get());
        CompositeExprPtr ce = interpreter->factory()->create_molecule();
        if (!ce) {
            return false;
        }

        CompositeExpr * operands = static_cast<CompositeExpr *>(proc_operands.get());
        operands->rewind();
        while (operands->has_next()) {
            MatterPtr res = NULL;
            if (!_realize(operands->get_next(), res)) {
                return false;
            }
            ce->append_expr(res);
        }
        return p->check_operands(ce)
                && p->run(ce, result, interpreter->factory());
    }

    if (proc_name->is_proc()) {
        Proc * proc = static_cast<Proc *>(proc_name.get());
        MatterPtr temp = NULL;
        ScopedEnvPtr env = NULL;
        if (!proc->get_params(temp) || !proc->get_env(env)) {
            return false;
        }

        MatterPtr body = NULL;
        if (!proc->get_body(body)) {
            return false;
        }

        CompositeExpr * params = static_cast<CompositeExpr *>(temp.get());
        CompositeExpr * operands =
                static_cast<CompositeExpr *>(proc_operands.get());
        if (params->size() != operands->size()) {
            return false;
        }
        if (!params->rewind() || !operands->rewind()) {
            return false;
        }

        ScopedEnvPtr newenv = interpreter->factory()->create_env(env);
        if (!newenv) {
            return false;
        }

        Expr * param = NULL;
        while (params->has_next() /* && operands->has_next() */) {
            param = static_cast<Expr*>(params->get_next().get());
            if (!param->is_cstr() || param->is_quoted_cstr()) {
                return false;
            }

            if (!newenv->add(param->to_cstr(), operands->get_next())) {
                return false;
            }
        }

        return _eval(body, newenv, interpreter, result);
    }

    return false;
}

bool InterpreterIF::execute(MatterPtr &result, const char * str, ssize_t len)
{
    MatterPtr forms = _parser->parse(str, len);
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

bool InterpreterIF::execute_multi_expr(MatterPtr &result, const MatterPtr &expr)
{
    if (!expr->is_molecule()) {
        PRETTY_MESSAGE(stderr, "expr is not CompositeExpr!");
        return false;
    }

    CompositeExpr * forms = static_cast<CompositeExpr *>(expr.get());
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

bool InterpreterIF::execute_expr(MatterPtr &result, const MatterPtr &expr)
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

    MatterPtr ie = NULL;
    CompositeExpr * ce = NULL;

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

        ce = static_cast<CompositeExpr *>(ie.get());
        ce->rewind();
        while (ce->has_next()) {
            MatterPtr next = ce->get_next();
            PRETTY_MESSAGE(stderr, "executing expr `%s' ...",
                    next->debug_string(false).c_str());
            MatterPtr a_result = NULL;
            if (!interpreter->execute_expr(a_result, next)) {
                PRETTY_MESSAGE(stderr, "oops, failed!");
            }
            else if (a_result) {
                PRETTY_MESSAGE(stderr, "result: `%s'",
                        a_result->debug_string().c_str());
                printf("%s\n", a_result->debug_string().c_str());
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
