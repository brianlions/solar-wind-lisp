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
        if (!(_env = new (std::nothrow) ScopedEnv())) {
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

void InterpreterIF::_repl(InterpreterIF * interpreter, bool continue_on_error)
{
}

void InterpreterIF::interactive(bool continue_on_error)
{
    InterpreterIF::_repl(this, continue_on_error);
}

void InterpreterIF::repl(bool continue_on_error)
{
    InterpreterIF * interpreter = new SimpleInterpreter();
    InterpreterIF::_repl(interpreter, continue_on_error);
    delete interpreter;
}

} // namespace SolarWindLisp
