/*
 * file name:           src/protobuf_utils.cc
 *
 * author:              Brian Yi ZHANG
 * email:               brianlions@gmail.com
 * date created:        Thu Jun 11 22:07:19 2015 CST
 */

#include <string.h>
#include <iostream>
#include "protobuf_utils.h"

#define COMPLAIN(fmt, ...)

namespace SolarWindLisp
{
namespace ProtobufUtils
{

const int ReturnCode::ERROR;
const int ReturnCode::SUCCESS;

int ReflectionMessageFactory::initialize(const char * proto_file)
{
    // disallow multiple initialization
    if (_initialized || !proto_file) {
        return ReturnCode::ERROR;
    }

    const char * dirname = NULL;
    const char * filename = NULL;
    // FIXME: use strlcpy
    strncpy(_proto_filepath, proto_file, sizeof(_proto_filepath));
    char * last_dir_sep = strrchr(_proto_filepath, '/');
    if (!last_dir_sep) {
        dirname = ".";
        filename = _proto_filepath;
    }
    else if (!*(last_dir_sep + 1)) {
        // ends with dir sep, no filename
        return ReturnCode::ERROR;
    }
    else if (last_dir_sep == _proto_filepath) {
        dirname = "/";
        filename = last_dir_sep + 1;
    }
    else {
        *last_dir_sep = '\0';
        dirname = _proto_filepath;
        filename = last_dir_sep + 1;
    }

    if (!(_dynamic_message_factory = new (std::nothrow) DynamicMessageFactory)
        || !(_disk_source_tree = new (std::nothrow) DiskSourceTree())
        || !(_importer = new (std::nothrow) Importer(_disk_source_tree, NULL))) {
        _clear();
        return ReturnCode::ERROR;
    }

    _disk_source_tree->MapPath("", dirname);
    _importer->Import(filename);
    _initialized = true;
    return ReturnCode::SUCCESS;
}

const Message * ReflectionMessageFactory::get_default_message(
        const char * msg_name) const
{
    typedef std::map<std::string, const Message *>::const_iterator CI;
    CI iter = _cache.find(msg_name);
    if (iter != _cache.end()) {
        return iter->second;
    }

    const Descriptor * descriptor =
        _importer->pool()->FindMessageTypeByName(msg_name);
    if (!descriptor) {
        return NULL;
    }

    const Message * default_message =
        _dynamic_message_factory->GetPrototype(descriptor);
    if (!default_message) {
        return NULL;
    }

    _cache[msg_name] = default_message;
    return default_message;
}

void ReflectionMessageFactory::_clear()
{
    if (_dynamic_message_factory) {
        delete _dynamic_message_factory;
        _dynamic_message_factory = NULL;
    }

    if (_disk_source_tree) {
        delete _disk_source_tree;
        _disk_source_tree = NULL;
    }

    if (_importer) {
        delete _importer;
        _importer = NULL;
    }
}

int Dissector::dissect(Dissector::field_value_t * result, const Message * msg,
        const char * spec, int index)
{
    // FIXME: use strlcpy
    strncpy(_spec_buf, spec, sizeof(_spec_buf));
    _string_tokens.split(_spec_buf, ".");

    const Descriptor * descriptor = NULL;
    const FieldDescriptor * field_descriptor = NULL;
    const Reflection * reflection = NULL;

    const Message * current = msg;
    uint32_t n_tokens = _string_tokens.num_of_tokens();
    // unsigned underflow is NOT a problem
    uint32_t last_pos = n_tokens - 1;
    for (uint32_t i = 0; i < n_tokens; ++i) {
        if (!(descriptor = current->GetDescriptor())) {
            COMPLAIN("current->GetDescriptor() failed");
            return ReturnCode::ERROR;
        }
        if (!(field_descriptor = descriptor->FindFieldByName(
                _string_tokens.token(i)))) {
            COMPLAIN("descriptor->FindFieldByName(\"%s\")",
                    _string_tokens.token(i));
            return ReturnCode::ERROR;
        }
        if (!(reflection = current->GetReflection())) {
            COMPLAIN("current->GetReflection() failed");
            return ReturnCode::ERROR;
        }

        if (i != last_pos) {
            // not last field in spec
            if (field_descriptor->cpp_type()
                    != FieldDescriptor::CPPTYPE_MESSAGE) {
                COMPLAIN("field %s (pos %u): not a message",
                        _string_tokens.token(i), i);
                return ReturnCode::ERROR;
            }
            if (field_descriptor->is_repeated()) {
                COMPLAIN("field %s (pos %u): internal field must not be repeated",
                        _string_tokens.token(i), i);
                return ReturnCode::ERROR;
            }
            current = &(reflection->GetMessage(*current, field_descriptor));
            continue;
        }

        // negative index is acceptable
        // e.g. -1: the last, -2: second to the last
#define ES_FUNKTIONIERT(r, e, t, f)                                     \
case FieldDescriptor::CPPTYPE_##e:                                      \
    if (field_descriptor->is_repeated()) {                              \
        int s = reflection->FieldSize(*current, field_descriptor);      \
        if (index >= s || (index < 0 && (s + index) < 0)) {             \
            COMPLAIN("field %s (pos %u): index %d is out of range",     \
                    _string_tokens.token(i), i, index);                 \
            return ReturnCode::ERROR;                                   \
        }                                                               \
        result->data._##f = reflection->GetRepeated##t(*current,        \
                field_descriptor, index >= 0 ? index : s + index);      \
    }                                                                   \
    else {                                                              \
        result->data._##f = reflection->Get##t(*current, field_descriptor); \
    }                                                                   \
    result->type = FieldDescriptor::CPPTYPE_##e;                        \
    break;

        switch (field_descriptor->cpp_type()) {
            ES_FUNKTIONIERT(int32_t,  INT32,  Int32,     i32);
            ES_FUNKTIONIERT(uint32_t, UINT32, UInt32,    u32);
            ES_FUNKTIONIERT(int64_t,  INT64,  Int64,     i64);
            ES_FUNKTIONIERT(uint64_t, UINT64, UInt64,    u64);
            ES_FUNKTIONIERT(double,   DOUBLE, Double,    double);
            ES_FUNKTIONIERT(float,    FLOAT,  Float,     float);
            ES_FUNKTIONIERT(bool,     BOOL,   Bool,      bool);
#if 0
            // Reflection::GetEnumValue() & GetRepeatedEnumValue() is not
            // supported in older version of protobuf?
            ES_FUNKTIONIERT(int,      ENUM,   EnumValue, i32);
#endif
#undef ES_FUNKTIONIERT
            case FieldDescriptor::CPPTYPE_ENUM:
                if (field_descriptor->is_repeated()) {
                    int s = reflection->FieldSize(*current, field_descriptor);
                    if (index >= s || (index < 0 && (s + index) < 0)) {
                        COMPLAIN("field %s (pos %u): index %d is out of range",
                                _string_tokens.token(i), i, index);
                        return ReturnCode::ERROR;
                    }
                    result->data._i32 = reflection->GetRepeatedEnum(*current,
                            field_descriptor,
                            index >= 0 ? index : s + index)->number();
                }
                else {
                    result->data._i32 = reflection->GetEnum(*current,
                            field_descriptor)->number();
                }
                result->type = FieldDescriptor::CPPTYPE_ENUM;
                break;
            case FieldDescriptor::CPPTYPE_STRING:
                if (field_descriptor->is_repeated()) {
                    int s = reflection->FieldSize(*current, field_descriptor);
                    if (index >= s || (index < 0 && (s + index) < 0)) {
                        COMPLAIN("field %s (pos %u): index %d is out of range",
                                _string_tokens.token(i), i, index);
                        return ReturnCode::ERROR;
                    }
                    result->data._str = reflection->GetRepeatedStringReference(
                            *current, field_descriptor,
                            index >= 0 ? index : s + index, NULL).c_str();
                }
                else {
                    result->data._str = reflection->GetStringReference(*current,
                            field_descriptor, NULL).c_str();
                }
                result->type = FieldDescriptor::CPPTYPE_STRING;
                break;
            case FieldDescriptor::CPPTYPE_MESSAGE:
                if (field_descriptor->is_repeated()) {
                    int s = reflection->FieldSize(*current, field_descriptor);
                    if (index >= s || (index < 0 && (s + index) < 0)) {
                        COMPLAIN("field %s (pos %u): index %d is out of range",
                                _string_tokens.token(i), i, index);
                        return ReturnCode::ERROR;
                    }
                    result->data._msg = &(reflection->GetRepeatedMessage(
                            *current, field_descriptor,
                            index >= 0 ? index : s + index));
                }
                else {
                    result->data._msg = &(reflection->GetMessage(*current,
                            field_descriptor));
                }
                result->type = FieldDescriptor::CPPTYPE_MESSAGE;
                break;
            default:
                COMPLAIN("this is ridiculous!");
                return ReturnCode::ERROR;
                break;
        } // end of switch
    } // end of for

    return ReturnCode::SUCCESS;
} // end of Dissector::dissect()

} // namespace ProtobufUtils
} // namespace SolarWindLisp
