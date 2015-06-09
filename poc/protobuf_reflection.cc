#include <stdint.h>
#include <string>
#include <iostream>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/compiler/importer.h>
#include "foo.pb.h"
#include "string_tokens.h"

using google::protobuf::Message;
using google::protobuf::Reflection;
using google::protobuf::Descriptor;
using google::protobuf::FieldDescriptor;
using google::protobuf::FileDescriptor;
using google::protobuf::DynamicMessageFactory;
using google::protobuf::compiler::Importer;
using google::protobuf::compiler::DiskSourceTree;

const std::string horizontal_line = "------------------------------------------------------------\n";

void serialize_bar(std::string * ptr_data);
Bar * init_bar();
void reflection_demo(const std::string * ptr_data);
void string_split_test();
void unnamed();
int protobuf_reader(const Message * message, const char * spec, int index = 0);

#define COMPLAIN(fmt, ...)                                                      \
do {                                                                            \
    fprintf(stderr, "[%s:%d:%s] " fmt "\n", __FILE__, __LINE__, __FUNCTION__,   \
        ## __VA_ARGS__);                                                        \
} while (0)

int main(int argc, char ** argv)
{
#if 0
    unnamed();
#endif
    string_split_test();

    std::string data;
    serialize_bar(&data);
    reflection_demo(&data);
    return 0;
}

void serialize_bar(std::string * ptr_data)
{
    Bar * bar = init_bar();
    std::cout << bar->DebugString() << std::endl;
    bar->SerializeToString(ptr_data);
    delete bar;
}

void reflection_demo(const std::string * ptr_data)
{
    // TODO:
    //   the dir name, file name, and message name are hard-coded, use function
    //   parameters instead in the future.
    DiskSourceTree disk_source_tree;
    disk_source_tree.MapPath("", "./proto"); // dir name

    Importer importer(&disk_source_tree, NULL);
    importer.Import("foo.proto"); // file name

    const Descriptor * descriptor =
        importer.pool()->FindMessageTypeByName("Bar"); // message name
    if (!descriptor) {
        fprintf(stderr, "line %d: oops!", __LINE__);
        return;
    }

    DynamicMessageFactory dynamic_message_factory;
    const Message * default_message =
        dynamic_message_factory.GetPrototype(descriptor);
    if (!default_message) {
        fprintf(stderr, "line %d: oops!", __LINE__);
        return;
    }

    Message * message = default_message->New();
    message->ParseFromString(*ptr_data);
    std::cout
        << horizontal_line
        << "deserialized message:\n``" << message->DebugString() << "''"
        << std::endl;

    struct ReaderArgs {
        const char * spec;
        int index;
    } args[] = {
        { "bar_name", 0 },
        { "bar_number", 0 },

        { "bar_foo", 0 },
        { "bar_foo.text", 0 },
        { "bar_foo.number", 0 },
        { "bar_foo.numbers", 0 },
        { "bar_foo.numbers", 1 },
        { "bar_foo.numbers", 2 },
        { "bar_foo.numbers", -1 },
        { "bar_foo.numbers", -2 },
        { "bar_foo.numbers", -3 },

        { "bar_numbers", 0 },
        { "bar_numbers", 1 },
        { "bar_numbers", 2 },
        { "bar_numbers", 3 },
        { "bar_numbers", -1 },
        { "bar_numbers", -2 },
        { "bar_numbers", -3 },
        { "bar_numbers", -4 },

        { "bar_foos", 0 },
        { "bar_foos", 1 },
        { "bar_foos", 2 },
        { "bar_foos", -1 },
        { "bar_foos", -2 },
        { "bar_foos", -3 },

        { "no_such_field", 0 },
        { "no_such_field", 1 },
        { "no.such.field", 0 },
        { "no.such.field", 1 },
        { "no.such.field", -1 },
    };

    for (uint32_t i = 0; i < sizeof(args)/sizeof(args[0]); ++i) {
        protobuf_reader(message, args[i].spec, args[i].index);
    }

    delete message;
}

int protobuf_reader(const Message * message, const char * spec, int index)
{
    printf("[*] spec: `%s', index: %d\n", spec, index);

    char spec_buf[1024];
    strncpy(spec_buf, spec, sizeof(spec_buf));
    SolarWindLisp::StringTokens tokens;
    tokens.split(spec_buf, ".");

    const Descriptor * descriptor = NULL;
    const FieldDescriptor * field_descriptor = NULL;
    const Reflection * reflection = NULL;

    const Message * current = message;
    uint32_t n_tokens = tokens.num_of_tokens();
    // unsigned underflow is NOT a problem
    uint32_t last_pos = n_tokens - 1;
    for (uint32_t i = 0; i < n_tokens; ++i) {
        if (!(descriptor = current->GetDescriptor())) {
            COMPLAIN("current->GetDescriptor() failed");
            return -1;
        }
        if (!(field_descriptor = descriptor->FindFieldByName(tokens.token(i)))) {
            COMPLAIN("descriptor->FindFieldByName(\"%s\")", tokens.token(i));
            return -1;
        }
        if (!(reflection = current->GetReflection())) {
            COMPLAIN("current->GetReflection() failed");
            return -1;
        }

        if (i != last_pos) {
            // not last field in spec
            if (field_descriptor->cpp_type() != FieldDescriptor::CPPTYPE_MESSAGE) {
                COMPLAIN("field %s (pos %u): not a message", tokens.token(i), i);
                return -1;
            }
            if (field_descriptor->is_repeated()) {
                COMPLAIN("field %s (pos %u): internal field must not be repeated", tokens.token(i), i);
                return -1;
            }
            current = &(reflection->GetMessage(*current, field_descriptor));
            continue;
        }

        // negative index is acceptable, e.g. -1: last, -2: second to the last
#define DAS_IST_TOLL(r, e, t)                                                                           \
        case FieldDescriptor::CPPTYPE_##e:                                                              \
            if (field_descriptor->is_repeated()) {                                                      \
                int s = reflection->FieldSize(*current, field_descriptor);                              \
                if (index >= s || (index < 0 && (s + index) < 0)) {                                     \
                    COMPLAIN("field %s (pos %u): index %d is out of range",                             \
                            tokens.token(i), i, index);                                                 \
                    return -1;                                                                          \
                }                                                                                       \
                r v = reflection->GetRepeated##t(*current, field_descriptor,                            \
                        index >= 0 ? index : s + index);                                                \
                std::cout << "\t(`" << spec << "', " << index << ") => " << v << std::endl;             \
            }                                                                                           \
            else {                                                                                      \
                r v = reflection->Get##t(*current, field_descriptor);                                   \
                std::cout << "\t(`" << spec << "', " << index << ") => " << v << std::endl;             \
            }                                                                                           \
            break;
        switch (field_descriptor->cpp_type()) {
            DAS_IST_TOLL(int32_t,     INT32,  Int32);
            DAS_IST_TOLL(uint32_t,    UINT32, UInt32);
            DAS_IST_TOLL(int64_t,     INT64,  Int64);
            DAS_IST_TOLL(uint64_t,    UINT64, UInt64);
            DAS_IST_TOLL(double,      DOUBLE, Double);
            DAS_IST_TOLL(float,       FLOAT,  Float);
            DAS_IST_TOLL(bool,        BOOL,   Bool);
            DAS_IST_TOLL(std::string, STRING, String);
#undef DAS_IST_TOLL
            case FieldDescriptor::CPPTYPE_ENUM:
                COMPLAIN("not implemented");
                return -1;
                break;
            case FieldDescriptor::CPPTYPE_MESSAGE:
                if (field_descriptor->is_repeated()) {
                    int s = reflection->FieldSize(*current, field_descriptor);
                    if (index >= s || (index < 0 && (s + index) < 0)) {
                        COMPLAIN("field %s (pos %u): index %d is out of range", tokens.token(i), i, index);
                        return -1;
                    }
                    const Message * m = &(reflection->GetRepeatedMessage(
                                *current, field_descriptor, index >= 0 ? index : s + index));
                    std::cout << "\t(`" << spec << "', " << index << ") => "
                        << m->ShortDebugString() << std::endl;
                }
                else {
                    const Message * m = &(reflection->GetMessage(*current, field_descriptor));
                    std::cout << "\t(`" << spec << "', " << index << ") => "
                        << m->ShortDebugString() << std::endl;
                }
                break;
            default:
                COMPLAIN("this is ridiculous!");
                return -1;
                break;
        } // end of switch

    } // end of for

    return 0;
}

Bar * init_bar()
{
    Bar * result = new Bar();
    result->set_bar_name("sie");
    result->set_bar_number(11);

    Foo * foo = result->mutable_bar_foo();
    foo->set_text("hat");
    foo->set_number(22);
    foo->add_numbers(33);
    foo->add_numbers(44);
    foo->add_numbers(55);

    result->add_bar_numbers(66);
    result->add_bar_numbers(77);
    result->add_bar_numbers(88);

    foo = result->add_bar_foos();
    foo->set_text("ein");
    foo->set_number(99);
    foo->add_numbers(1010);
    foo->add_numbers(1111);
    foo->add_numbers(1212);

    foo = result->add_bar_foos();
    foo->set_text("unfall");
    foo->set_number(1313);
    foo->add_numbers(1414);
    foo->add_numbers(1515);

    return result;
}

void string_split_test()
{
    SolarWindLisp::StringTokens st;
    char sentence[] = "deutsche.sprache.schwere.sprache";
    assert(st.split(sentence, "."));
    assert(st.num_of_tokens() == 4);
    const char * words[] = { "deutsche", "sprache", "schwere", "sprache" };
    for (int i = 0; i < 4; ++i) {
        assert(strcmp(st.token(i), words[i]) == 0);
    }
}

void unnamed()
{
    std::string data;  // Will store a serialized version of the message.

    {
        // Create a message and serialize it.
        Foo foo;
        foo.set_text("Hello World!");
        foo.add_numbers(1);
        foo.add_numbers(5);
        foo.add_numbers(42);

        foo.SerializeToString(&data);
    }

    {
        // Parse the serialized message and check that it contains the correct data.
        Foo foo;
        foo.ParseFromString(data);

        assert(foo.text() == "Hello World!");
        assert(foo.numbers_size() == 3);
        assert(foo.numbers(0) == 1);
        assert(foo.numbers(1) == 5);
        assert(foo.numbers(2) == 42);

        {
            std::cout
                << "[*] before Clear()" << std::endl
                << "DebugString():          " << foo.DebugString() << std::endl
                << "ShortDebugString():     " << foo.ShortDebugString() << std::endl
                << "GetTypeName():          " << foo.GetTypeName() << std::endl
                << "ByteSize():             " << foo.ByteSize() << std::endl
                << "SpaceUsed():            " << foo.SpaceUsed() << std::endl
                ;
            foo.Clear();
            std::cout
                << "[*] after Clear()" << std::endl
                << "DebugString():          " << foo.DebugString() << std::endl
                << "ShortDebugString():     " << foo.ShortDebugString() << std::endl
                << "GetTypeName():          " << foo.GetTypeName() << std::endl
                << "ByteSize():             " << foo.ByteSize() << std::endl
                << "SpaceUsed():            " << foo.SpaceUsed() << std::endl
                ;
        }

        Message * m2 = foo.New();
        std::cout
            << "m2:" << std::endl
            << "GetTypeName():          " << m2->GetTypeName() << std::endl
            << "ByteSize():             " << m2->ByteSize() << std::endl
            << "SpaceUsed():            " << m2->SpaceUsed() << std::endl
            ;
        delete m2;
    }

    {
        // Same as the last block, but do it dynamically via the Message reflection interface.
        Message* foo = new Foo;
        foo->ParseFromString(data);

        const Descriptor* descriptor = foo->GetDescriptor();
        std::cout
            << horizontal_line
            << "[*] descriptor" << std::endl
            << "full_name(): " << descriptor->full_name() << std::endl
            << "DebugString(): " << descriptor->DebugString() << std::endl
            << "field_count(): " << descriptor->field_count() << std::endl
            ;
        // Get the descriptors for the fields we're interested in and verify their types.
        const FieldDescriptor* text_field = descriptor->FindFieldByName("text");
        assert(text_field != NULL);
        std::cout
            << horizontal_line
            << "[*] text_field" << std::endl
            << "full_name(): " << text_field->full_name() << std::endl
            << "number(): " << text_field->number() << std::endl
            << "DebugString(): " << text_field->DebugString() << std::endl
            ;
        assert(text_field->type() == FieldDescriptor::TYPE_STRING);
        assert(text_field->label() == FieldDescriptor::LABEL_OPTIONAL);
        const FieldDescriptor* numbers_field = descriptor->FindFieldByName("numbers");
        std::cout
            << horizontal_line
            << "[*] numbers_field" << std::endl
            << "full_name(): " << numbers_field->full_name() << std::endl
            << "DebugString(): " << numbers_field->DebugString() << std::endl
            ;
        assert(numbers_field != NULL);
        assert(numbers_field->type() == FieldDescriptor::TYPE_INT32);
        assert(numbers_field->label() == FieldDescriptor::LABEL_REPEATED);

        const FileDescriptor * file_descriptor = descriptor->file();
        const Descriptor * foo_descriptor = file_descriptor->FindMessageTypeByName("Foo");
        assert(file_descriptor != NULL);
        assert(foo_descriptor != NULL);
        std::cout
            << horizontal_line
            << "[*] file_descriptor" << std::endl
            << "DebugString(): " << file_descriptor->DebugString() << std::endl
            << horizontal_line
            << "[*] foo_descriptor" << std::endl
            << "DebugString(): " << foo_descriptor->DebugString() << std::endl
            ;


        // Use the reflection interface to examine and modify the contents
        const Reflection* reflection = foo->GetReflection();

        // 1. optional string field
        assert(reflection->HasField(*foo, text_field) == true);
        assert(reflection->GetString(*foo, text_field) == "Hello World!");

        reflection->ClearField(foo, text_field);
        assert(reflection->HasField(*foo, text_field) == false);

        reflection->SetString(foo, text_field, "hello protobuf");
        assert(reflection->HasField(*foo, text_field) == true);
        assert(reflection->GetString(*foo, text_field) == "hello protobuf");

        // 2. repeated int32 field
        assert(reflection->FieldSize(*foo, numbers_field) == 3);
        assert(reflection->GetRepeatedInt32(*foo, numbers_field, 0) == 1);
        assert(reflection->GetRepeatedInt32(*foo, numbers_field, 1) == 5);
        assert(reflection->GetRepeatedInt32(*foo, numbers_field, 2) == 42);

        reflection->RemoveLast(foo, numbers_field);
        assert(reflection->FieldSize(*foo, numbers_field) == 2);
        reflection->SwapElements(foo, numbers_field, 0, 1);
        assert(reflection->GetRepeatedInt32(*foo, numbers_field, 0) == 5);
        assert(reflection->GetRepeatedInt32(*foo, numbers_field, 1) == 1);

        delete foo;
    }
}
