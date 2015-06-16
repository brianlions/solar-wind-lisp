/*
 * file name:           src/protobuf_heavy_interp.cc
 *
 * author:              Brian Yi ZHANG
 * email:               brianlions@gmail.com
 * date created:        Sun Jun 14 12:16:27 2015 CST
 */

#include "types.h"
#include "protobuf_heavy_interp.h"
#include "convertor.h"

namespace SolarWindLisp
{

using ProtobufUtils::ReturnCode;

int ProtobufHeavyInterpreter::reflection_initialize(const char * proto_filename)
{
    return _rmf.initialize(proto_filename);
}

int ProtobufHeavyInterpreter::set_message(const void * data, int size,
        const char * name)
{
    if (_initialized) {
        return ReturnCode::ERROR;
    }

    if (!(_message = _rmf.create_message(name))) {
        return ReturnCode::ERROR;
    }

    if (!_message->ParseFromArray(data, size)) {
        delete _message;
        return ReturnCode::ERROR;
    }

    _initialized = true;
    return ReturnCode::SUCCESS;
}

int ProtobufHeavyInterpreter::customize()
{
    ScopedEnvPtr env = this->env();

    PrimProcPtr pp = PrimProcGetRequestMessage::create(this);
    if (!pp) {
        return ReturnCode::ERROR;
    }

    if (!env->add(pp->name(), pp)) {
        return ReturnCode::ERROR;
    }

    prim_proc_ptr_creator_func_t items[] = { //
            PrimProcProtobufReader::create, //
            };
    for (size_t i = 0; i < array_size(items); ++i) {
        PrimProcPtr ptr = items[i]();
        if (!ptr) {
            return ReturnCode::ERROR;
        }

        if (!env->add(ptr->name(), ptr)) {
            return ReturnCode::ERROR;
        }
    }

    typedef std::vector<prim_proc_ptr_creator_func_t>::const_iterator CI;
    if (_creator_funcs) {
        for (CI iter = _creator_funcs->begin(); iter != _creator_funcs->end();
                ++iter) {
            PrimProcPtr ptr = (*iter)();
            if (!ptr) {
                return ReturnCode::ERROR;
            }

            if (!env->add(ptr->name(), ptr)) {
                return ReturnCode::ERROR;
            }
        }
    }

    return ReturnCode::SUCCESS;
}

int ProtobufHeavyInterpreter::pre_initialization(const void * data, int size,
        const char * proto_filename, const char * name,
        const std::vector<prim_proc_ptr_creator_func_t> * funcs)
{
    _creator_funcs = funcs;
    return (reflection_initialize(proto_filename) == ReturnCode::SUCCESS
            && set_message(data, size, name) == ReturnCode::SUCCESS) ?
            ReturnCode::SUCCESS : ReturnCode::ERROR;
}

bool PrimProcGetRequestMessage::run(const MatterPtr &ops, MatterPtr &result,
        MatterFactoryIF * factory)
{
    AtomPtr res = factory->create_atom();
    if (!res) {
        return false;
    }

    const Message * m = _pb_heavy_interp->get_message();
    if (!m) {
        return false;
    }

    //std::cout << "RequestMessage [" << ((void *) m) << "]: " << m->ShortDebugString() << std::endl;
    res->set_pointer(m);
    result = res;
    return true;
}

bool PrimProcProtobufReader::run(const MatterPtr &ops, MatterPtr &result,
        MatterFactoryIF * factory)
{
    const CompositeExpr * ce = static_cast<const CompositeExpr *>(ops.get());
    if (!ce->rewind()) {
        return false;
    }

    AtomPtr res = factory->create_atom();
    if (!res.get()) {
        return false;
    }

    const Atom * message = static_cast<const Atom *>(ce->get(0).get());
    const Atom * spec = static_cast<const Atom *>(ce->get(1).get());
    if (!message->is_pointer() || !spec->is_cstr()) {
        return false;
    }

    const void * msg_ptr = NULL;
    if (!message->to_pointer(msg_ptr)) {
        return false;
    }

    int index = 0;
    if (ce->size() == 3
            && !static_cast<const Atom *>(ce->get(2).get())->to_i32(index)) {
        return false;
    }

    ProtobufUtils::Dissector::field_value_t temp;
    if (_dissector.dissect(&temp, static_cast<const Message *>(msg_ptr),
            spec->to_cstr(), index) != ReturnCode::SUCCESS) {
        return false;
    }

    if (!Convertor::convert(&temp, res)) {
        return false;
    }

    result = res;
    return true;
}

} // namespace SolarWindLisp
