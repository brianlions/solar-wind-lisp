#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <string>
#include <google/protobuf/message.h>
#include "pretty_message.h"
#include "protobuf_utils.h"
#include "utils.h"
#include "foo.pb.h"

using google::protobuf::Message;
using google::protobuf::FieldDescriptor;
using SolarWindLisp::ProtobufUtils::Dissector;
using SolarWindLisp::ProtobufUtils::ReturnCode;
using SolarWindLisp::ProtobufUtils::ReflectionMessageFactory;
using SolarWindLisp::Utils::Time;

extern int foo_build_and_serialize(void * buf, int size,
        Bar ** ptr_msg = NULL);

struct ReflectionArgs {
    const char * spec;
    int index;
    FieldDescriptor::CppType type;
};

void profiling_it(const Message * message, const ReflectionArgs * ra,
        size_t ra_len, uint32_t n_times)
{
    if (!n_times) {
        return;
    }

    Dissector dissector;
    Dissector::field_value_t field;
    for (size_t i = 0; i < ra_len; ++i) {
        bool found_error = false;
        const char * spec = (ra + i)->spec;
        int index = (ra + i)->index;
        FieldDescriptor::CppType type = (ra + i)->type;
        int64_t start_time = Time::timestamp_usec();
        uint32_t cnt = 0;
        for (uint32_t j = 0; j < n_times; ++j) {
            if (dissector.dissect(&field, message, spec, index)
                    != ReturnCode::SUCCESS || field.type != type) {
                found_error = true;
                break;
            }
            cnt++;
        }
        int64_t finish_time = Time::timestamp_usec();
        if (!found_error && cnt == n_times) {
            double avg = 1.0 * (finish_time - start_time) / n_times;
            fprintf(stdout,
                    "[+] %lu: spec `%s', index: `%d'\n"
                    "  %10.6f usec (%lld / %u)\n",
                    i, spec, index,
                    avg, (finish_time - start_time), cnt);
        }
        else {
            fprintf(stderr,
                    "[-] %lu: spec `%s', index: `%d' failed!\n",
                    i, spec, index);
        }
    }
}

void profiling_it(const Bar * message, uint32_t n_times)
{

#define was_ist_das_begin()                                             \
{                                                                       \
    int64_t start_time = Time::timestamp_usec();                        \
    uint32_t cnt = 0;                                                   \
    bool found_error = false;                                           \
    for (uint32_t i = 0; i < n_times; ++i) {

#define was_ist_das_end(msg)                                            \
        cnt++;                                                          \
    }                                                                   \
    int64_t finish_time = Time::timestamp_usec();                       \
    if (!found_error && cnt == n_times) {                               \
        double avg = 1.0 * (finish_time - start_time) / n_times;        \
        fprintf(stdout, "[+] `%s'\n"                                    \
                        "    %10.6f (%lld / %u)\n", msg,                \
                        avg, (finish_time - start_time), n_times);      \
    }                                                                   \
}

    was_ist_das_begin()
        std::string v = message->bar_name();
        if (!v.length()) {
            found_error = true;
            break;
        }
    was_ist_das_end("message->bar_name()")

    was_ist_das_begin()
        int32_t v = message->bar_number();
        if (!v) {
            found_error = true;
            break;
        }
    was_ist_das_end("message->bar_number()")

    was_ist_das_begin()
        const Foo &v = message->bar_foo();
    was_ist_das_end("message->bar_foo()")

    was_ist_das_begin()
        uint32_t v = message->bar_numbers(0);
        if (!v) {
            found_error = true;
            break;
        }
    was_ist_das_end("message->bar_numbers(0)");

    was_ist_das_begin()
        uint32_t v = message->bar_numbers(1);
        if (!v) {
            found_error = true;
            break;
        }
    was_ist_das_end("message->bar_numbers(1)");

    was_ist_das_begin()
        uint32_t v = message->bar_numbers(2);
        if (!v) {
            found_error = true;
            break;
        }
    was_ist_das_end("message->bar_numbers(2)");

    was_ist_das_begin()
        const Foo &v = message->bar_foos(0);
    was_ist_das_end("message->bar_foos(0)")

    was_ist_das_begin()
        const Foo &v = message->bar_foos(1);
    was_ist_das_end("message->bar_foos(1)")

#undef was_ist_das_begin
#undef was_ist_das_end
}

int main(int argc, char ** argv)
{
    char buffer[1024 * 16];
    int len = 0;
    Bar * bar_msg = NULL;
    if ((len = foo_build_and_serialize(buffer, sizeof(buffer), &bar_msg)) < 0) {
        fprintf(stderr, "foo_build_and_serialize() failed!\n");
        exit(EXIT_FAILURE);
    }

    ReflectionMessageFactory rmf;
    if (rmf.initialize("./proto/foo.proto") != ReturnCode::SUCCESS) {
        fprintf(stderr, "rmf.initialize() failed!\n");
        exit(EXIT_FAILURE);
    }

    Message * r_msg = rmf.create_message("Bar");
    if (!r_msg) {
        fprintf(stderr, "rmf.create_message() failed!\n");
        exit(EXIT_FAILURE);
    }

    if (!r_msg->ParseFromArray(buffer, len)) {
        fprintf(stderr, "r_msg->ParseFromArray() failed!\n");
        exit(EXIT_FAILURE);
    }

    ReflectionArgs bar_cases[] = { //
        { "bar_name",     0, FieldDescriptor::CPPTYPE_STRING }, //
        { "bar_number",   0, FieldDescriptor::CPPTYPE_INT32 }, //
        { "bar_foo",      0, FieldDescriptor::CPPTYPE_MESSAGE }, //
        { "bar_numbers",  0, FieldDescriptor::CPPTYPE_INT32 }, //
        { "bar_numbers",  1, FieldDescriptor::CPPTYPE_INT32 }, //
        { "bar_numbers",  2, FieldDescriptor::CPPTYPE_INT32 }, //
        { "bar_numbers", -1, FieldDescriptor::CPPTYPE_INT32 }, //
        { "bar_numbers", -2, FieldDescriptor::CPPTYPE_INT32 }, //
        { "bar_numbers", -3, FieldDescriptor::CPPTYPE_INT32 }, //
        { "bar_foos",     0, FieldDescriptor::CPPTYPE_MESSAGE }, //
        { "bar_foos",     1, FieldDescriptor::CPPTYPE_MESSAGE }, //
        { "bar_foos",    -1, FieldDescriptor::CPPTYPE_MESSAGE }, //
        { "bar_foos",    -2, FieldDescriptor::CPPTYPE_MESSAGE }, //
    };

    uint32_t n_times = 1000000;
    profiling_it(r_msg, bar_cases, array_size(bar_cases), n_times);
    profiling_it(bar_msg, n_times);

    delete r_msg;
    delete bar_msg;
    exit(EXIT_SUCCESS);
}
