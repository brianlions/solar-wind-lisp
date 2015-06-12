/*
 * file name:           include/protobuf_utils.h
 *
 * author:              Brian Yi ZHANG
 * email:               brianlions@gmail.com
 * date created:        Thu Jun 11 22:07:19 2015 CST
 */

#ifndef _SOLAR_WIND_LISP_PROTOBUF_UTILS_H_
#define _SOLAR_WIND_LISP_PROTOBUF_UTILS_H_

#include <string.h>
#include <string>
#include <map>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/compiler/importer.h>
#include "string_tokens.h"

using google::protobuf::Message;
using google::protobuf::Reflection;
using google::protobuf::Descriptor;
using google::protobuf::FieldDescriptor;
using google::protobuf::FileDescriptor;
using google::protobuf::DynamicMessageFactory;
using google::protobuf::compiler::Importer;
using google::protobuf::compiler::DiskSourceTree;

namespace SolarWindLisp
{
namespace ProtobufUtils
{

class ReturnCode
{
public:
    static const int ERROR = -1;
    static const int SUCCESS = 0;
};

class ReflectionMessageFactory
{
public:

    ReflectionMessageFactory() :
            _dynamic_message_factory(NULL), //
            _disk_source_tree(NULL), //
            _importer(NULL), //
            _initialized(false) //
    {
        _proto_filepath[0] = '\0';
    }

    virtual ~ReflectionMessageFactory()
    {
        _clear();
    }

    bool initialized() const
    {
        return _initialized;
    }

    int initialize(const char * proto_filename);

    const Message * get_default_message(const char * msg_name) const;

    Message * create_message(const char * message_name)
    {
        const Message * m = get_default_message(message_name);
        return m ? m->New() : NULL;
    }

private:
    void _clear();

    char _proto_filepath[2048];
    mutable std::map<std::string, const Message *> _cache;
    DynamicMessageFactory * _dynamic_message_factory;
    DiskSourceTree * _disk_source_tree;
    Importer * _importer;
    bool _initialized;
};

class Dissector
{
public:
    typedef struct FieldValue
    {
        FieldValue()
        {
            memset(&data, 0, sizeof(data));
            type = static_cast<FieldDescriptor::CppType>(0);
        }

        union
        {
            int32_t _i32;
            uint32_t _u32;
            int64_t _i64;
            uint64_t _u64;
            double _double;
            float _float;
            bool _bool;
            const char * _str;
            const Message * _msg;
        } data;

        FieldDescriptor::CppType type;
    } field_value_t;

    int dissect(field_value_t * result, const Message * msg, const char * spec,
            int index);

private:
    char _spec_buf[512];
    StringTokens _string_tokens;
};

} // namespace ProtobufUtils
} // namespace SolarWindLisp

#endif // _SOLAR_WIND_LISP_PROTOBUF_UTILS_H_
