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
extern int company_build_and_serialize(Company ** ptr_msg,
        void * &buf, int &size);

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
                    "\t%10.6f usec (%lld / %u)\n",
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

void foo_and_bar_perf()
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

    ReflectionArgs bar_cases[] = {
        { "bar_name",     0, FieldDescriptor::CPPTYPE_STRING },
        { "bar_number",   0, FieldDescriptor::CPPTYPE_INT32 },
        { "bar_foo",      0, FieldDescriptor::CPPTYPE_MESSAGE },
        { "bar_numbers",  0, FieldDescriptor::CPPTYPE_INT32 },
        { "bar_numbers",  1, FieldDescriptor::CPPTYPE_INT32 },
        { "bar_numbers",  2, FieldDescriptor::CPPTYPE_INT32 },
        { "bar_numbers", -1, FieldDescriptor::CPPTYPE_INT32 },
        { "bar_numbers", -2, FieldDescriptor::CPPTYPE_INT32 },
        { "bar_numbers", -3, FieldDescriptor::CPPTYPE_INT32 },
        { "bar_foos",     0, FieldDescriptor::CPPTYPE_MESSAGE },
        { "bar_foos",     1, FieldDescriptor::CPPTYPE_MESSAGE },
        { "bar_foos",    -1, FieldDescriptor::CPPTYPE_MESSAGE },
        { "bar_foos",    -2, FieldDescriptor::CPPTYPE_MESSAGE },
    };

    uint32_t n_times = 1000000;
    profiling_it(r_msg, bar_cases, array_size(bar_cases), n_times);
    profiling_it(bar_msg, n_times);

    delete r_msg;
    delete bar_msg;
}

void company_perf()
{
    Company * company = NULL;
    void * comp_buf = NULL;
    int32_t comp_buf_size = 0;
    if (!company_build_and_serialize(&company, comp_buf, comp_buf_size)) {
        fprintf(stderr, "company_build_and_serialize() failed!\n");
        exit(EXIT_FAILURE);
    }

    printf("size of serialized Company message: %d\n", comp_buf_size);

    // preparation
    ReflectionMessageFactory rmf;
    if (rmf.initialize("./proto/foo.proto") != ReturnCode::SUCCESS) {
        fprintf(stderr, "rmf.initialize() failed!\n");
        exit(EXIT_FAILURE);
    }

    Message * r_msg = rmf.create_message("Company");
    if (!r_msg) {
        fprintf(stderr, "rmf.create_message() failed!\n");
        exit(EXIT_FAILURE);
    }

    if (!r_msg->ParseFromArray(comp_buf, comp_buf_size)) {
        fprintf(stderr, "r_msg->ParseFromArray() failed!\n");
        exit(EXIT_FAILURE);
    }

    // profiling
    ReflectionArgs prof_cases[] = {
        { "name", 0,
            FieldDescriptor::CPPTYPE_STRING },
        { "description", 0,
            FieldDescriptor::CPPTYPE_STRING },
        { "company_id", 0,
            FieldDescriptor::CPPTYPE_INT32 },
        { "president", 0,
            FieldDescriptor::CPPTYPE_MESSAGE },
        { "president.name", 0,
            FieldDescriptor::CPPTYPE_STRING },
        { "president.age", 0,
            FieldDescriptor::CPPTYPE_INT32 },
        { "president.height", 0,
            FieldDescriptor::CPPTYPE_INT32 },
        { "president.weight", 0,
            FieldDescriptor::CPPTYPE_INT32 },
        { "president.address", 0,
            FieldDescriptor::CPPTYPE_STRING },
        { "president.seat", 0,
            FieldDescriptor::CPPTYPE_STRING },
        { "president.lucky_numbers", 0,
            FieldDescriptor::CPPTYPE_INT32 },
        { "president.lucky_numbers", 1,
            FieldDescriptor::CPPTYPE_INT32 },
        { "president.lucky_numbers", -1,
            FieldDescriptor::CPPTYPE_INT32 },
        { "president.lucky_numbers", -2,
            FieldDescriptor::CPPTYPE_INT32 },
        { "headquarter", 0,
            FieldDescriptor::CPPTYPE_MESSAGE },
        { "headquarter.name", 0,
            FieldDescriptor::CPPTYPE_STRING },
        { "headquarter.description", 0,
            FieldDescriptor::CPPTYPE_STRING },
        { "headquarter.department_id", 0,
            FieldDescriptor::CPPTYPE_INT32 },
        { "headquarter.manager", 0,
            FieldDescriptor::CPPTYPE_MESSAGE },
        { "headquarter.manager.name", 0,
            FieldDescriptor::CPPTYPE_STRING },
        { "headquarter.manager.age", 0,
            FieldDescriptor::CPPTYPE_INT32 },
        { "headquarter.manager.height", 0,
            FieldDescriptor::CPPTYPE_INT32 },
        { "headquarter.manager.weight", 0,
            FieldDescriptor::CPPTYPE_INT32 },
        { "headquarter.manager.address", 0,
            FieldDescriptor::CPPTYPE_STRING },
        { "headquarter.manager.seat", 0,
            FieldDescriptor::CPPTYPE_STRING },
        { "headquarter.manager.lucky_numbers", 0,
            FieldDescriptor::CPPTYPE_INT32 },
        { "headquarter.manager.lucky_numbers", 1,
            FieldDescriptor::CPPTYPE_INT32 },
        { "headquarter.manager.lucky_numbers", -1,
            FieldDescriptor::CPPTYPE_INT32 },
        { "headquarter.manager.lucky_numbers", -2,
            FieldDescriptor::CPPTYPE_INT32 },
    };

    uint32_t n_times = 1000000;
    profiling_it(r_msg, prof_cases, array_size(prof_cases), n_times);

    Dissector dissector;
    Dissector::field_value_t field;
    if (dissector.dissect(&field, r_msg, "headquarter", 0)
            == ReturnCode::SUCCESS
            && field.type == FieldDescriptor::CPPTYPE_MESSAGE) {
        ReflectionArgs hq_mngr_cases[] = {
            { "manager", 0,
                FieldDescriptor::CPPTYPE_MESSAGE },
            { "manager.name", 0,
                FieldDescriptor::CPPTYPE_STRING },
            { "manager.age", 0,
                FieldDescriptor::CPPTYPE_INT32 },
            { "manager.height", 0,
                FieldDescriptor::CPPTYPE_INT32 },
            { "manager.weight", 0,
                FieldDescriptor::CPPTYPE_INT32 },
            { "manager.address", 0,
                FieldDescriptor::CPPTYPE_STRING },
            { "manager.seat", 0,
                FieldDescriptor::CPPTYPE_STRING },
            { "manager.lucky_numbers", 0,
                FieldDescriptor::CPPTYPE_INT32 },
            { "manager.lucky_numbers", 1,
                FieldDescriptor::CPPTYPE_INT32 },
            { "manager.lucky_numbers", -1,
                FieldDescriptor::CPPTYPE_INT32 },
            { "manager.lucky_numbers", -2,
                FieldDescriptor::CPPTYPE_INT32 },
        };
        profiling_it(field.data._msg, hq_mngr_cases, array_size(hq_mngr_cases),
                n_times);
    }

    // cleanup
    delete company;
    free(comp_buf);
}

int main(int argc, char ** argv)
{
    foo_and_bar_perf();
    company_perf();
    exit(EXIT_SUCCESS);
}
