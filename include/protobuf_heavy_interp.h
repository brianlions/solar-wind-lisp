/*
 * file name:           include/protobuf_heavy_interp.h
 *
 * author:              Brian Yi ZHANG
 * email:               brianlions@gmail.com
 * date created:        Sun Jun 14 12:16:27 2015 CST
 */

#ifndef _SOLAR_WIND_LISP_PROTOBUF_HEAVY_INTERP_H_
#define _SOLAR_WIND_LISP_PROTOBUF_HEAVY_INTERP_H_

#include <vector>
#include "convertor.h"
#include "interpreter.h"
#include "protobuf_utils.h"
#include "prim_proc.h"
#include "types.h"

namespace SolarWindLisp
{

class ProtobufHeavyInterpreter: public InterpreterIF
{
public:
    ProtobufHeavyInterpreter(ParserIF * parser = NULL, ScopedEnvPtr env = NULL,
            MatterFactoryIF * factory = NULL) :
            InterpreterIF(parser, env, factory), //
            _creator_funcs(NULL), //
            _message(NULL), //
            _initialized(false)
    {
    }

    virtual ~ProtobufHeavyInterpreter()
    {
        if (_message) {
            delete _message;
        }
    }

    const Message * get_message() const
    {
        return _message;
    }

    virtual int customize();
    int pre_initialization(const void * data, int size,
            const char * proto_filename, const char * name,
            const std::vector<prim_proc_ptr_creator_func_t> * funcs = NULL);

private:
    int reflection_initialize(const char * proto_filename);
    int set_message(const void * data, int size, const char * name);

    const std::vector<prim_proc_ptr_creator_func_t> * _creator_funcs;
    Message * _message;
    ProtobufUtils::ReflectionMessageFactory _rmf;
    bool _initialized;
};

class PrimProcGetRequestMessage: public PrimProcIF
{
public:
    bool run(const MatterPtr &ops, MatterPtr &result,
            MatterFactoryIF * factory);

    bool check_operands(const MatterPtr &ops) const
    {
        return true; // always return true
    }

    const char * name() const
    {
        return "get-request-message";
    }

    std::string debug_string(bool compact = true, int level = 0,
            const char * indent_seq = DEFAULT_INDENT_SEQ) const
    {
        return "PrimProcGetRequestMessage{}";
    }

    std::string to_string() const
    {
        return "instance of PrimProcIF";
    }

    static PrimProcPtr create(const ProtobufHeavyInterpreter * pi)
    {
        return PrimProcPtr(new (std::nothrow) PrimProcGetRequestMessage(pi));
    }

private:
    PrimProcGetRequestMessage(const ProtobufHeavyInterpreter * interp) :
            _pb_heavy_interp(interp)
    {
    }

    const ProtobufHeavyInterpreter * _pb_heavy_interp;
};

class PrimProcProtobufReader: public PrimProcIF
{
public:
    bool run(const MatterPtr &ops, MatterPtr &result,
            MatterFactoryIF * factory);

    bool check_operands(const MatterPtr &ops) const
    {
        if (ops->is_composite_expr()) {
            size_t sz = static_cast<const CompositeExpr *>(ops.get())->size();
            return sz == 2 || sz == 3;
        }

        return false;
    }

    const char * name() const
    {
        return "protobuf-reader";
    }

    std::string debug_string(bool compact = true, int level = 0,
            const char * indent_seq = DEFAULT_INDENT_SEQ) const
    {
        return "PrimProcProtobufReader{}";
    }

    std::string to_string() const
    {
        return "instance of PrimProcIF";
    }

    static PrimProcPtr create()
    {
        return PrimProcPtr(new (std::nothrow) PrimProcProtobufReader());
    }

private:
    ProtobufUtils::Dissector _dissector;
};

} // namespace SolarWindLisp

#endif // _SOLAR_WIND_LISP_PROTOBUF_HEAVY_INTERP_H_
