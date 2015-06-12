#include <stdio.h>
#include <string>
#include <gtest/gtest.h>
#include "protobuf_utils.h"
#include "utils.h"
#include "foo.pb.h"

using SolarWindLisp::ProtobufUtils::ReflectionMessageFactory;
using SolarWindLisp::ProtobufUtils::Dissector;
using SolarWindLisp::ProtobufUtils::ReturnCode;

class ProtobufUtilsTS: public testing::Test
{
protected:
    static const char * temp_proto_contents;
    static const char * temp_proto_file;

    ReflectionMessageFactory msg_factory;
    Dissector dissector;
    std::string data;
    Message * message;

    virtual void SetUp()
    {
        EXPECT_EQ(write_proto_file(), 0);
        EXPECT_FALSE(msg_factory.initialized());
        EXPECT_EQ(msg_factory.initialize("./proto/foo.proto"), 0);
        EXPECT_TRUE(msg_factory.get_default_message("Foo"));
        EXPECT_TRUE(msg_factory.get_default_message("Bar"));
        EXPECT_FALSE(msg_factory.get_default_message("NoSuchMessageType"));
        build_and_serialize(&data);
        message = msg_factory.create_message("Bar");
        EXPECT_TRUE(message != NULL);
        EXPECT_TRUE(message->ParseFromString(data));
    }

    virtual void TearDown()
    {
        delete_proto_file();
        if (message) {
            delete message;
            message = NULL;
        }
    }

    int write_proto_file()
    {
        FILE * fh = fopen(temp_proto_file, "w+");
        if (!fh) {
            return -1;
        }

        fprintf(fh, "%s", temp_proto_contents);
        fclose(fh);
        return 0;
    }

    void delete_proto_file()
    {
        unlink(temp_proto_file);
    }

    void build_and_serialize(std::string * ptr_data)
    {
        Bar * a_message = new Bar();
        a_message->set_bar_name("sie");
        a_message->set_bar_number(11);

        Foo * foo = a_message->mutable_bar_foo();
        foo->set_text("hat");
        foo->set_number(22);
        foo->add_numbers(33);
        foo->add_numbers(44);
        foo->add_numbers(55);

        a_message->add_bar_numbers(66);
        a_message->add_bar_numbers(77);
        a_message->add_bar_numbers(88);

        foo = a_message->add_bar_foos();
        foo->set_text("ein");
        foo->set_number(99);
        foo->add_numbers(1010);
        foo->add_numbers(1111);
        foo->add_numbers(1212);

        foo = a_message->add_bar_foos();
        foo->set_text("unfall");
        foo->set_number(1313);
        foo->add_numbers(1414);
        foo->add_numbers(1515);

        a_message->SerializeToString(ptr_data);

        delete a_message;
    }
};

const char * ProtobufUtilsTS::temp_proto_file = "./temp_proto_file.proto";
const char * ProtobufUtilsTS::temp_proto_contents = "message Foo {\n"
        "    optional string text = 1;\n"
        "    optional int32 number = 2;\n"
        "    repeated int32 numbers = 3;\n"
        "}\n"
        "\n"
        "message Bar {\n"
        "    optional string bar_name = 1;\n"
        "    optional int32  bar_number = 2;\n"
        "    optional Foo bar_foo = 3;\n"
        "    repeated int32 bar_numbers = 4;\n"
        "    repeated Foo bar_foos = 5;\n"
        "}\n";

TEST_F(ProtobufUtilsTS, caseReflectionUnknown)
{

}

TEST_F(ProtobufUtilsTS, caseReflectionFieldString)
{
    struct ReflectString
    {
        const char * spec;
        int index;
        const char * string_value;
    } str_args[] = { //
            { "bar_name", 0, "sie" }, //
            { "bar_foo.text", 0, "hat" }, //
            };
    for (size_t i = 0; i < array_size(str_args); ++i) {
        Dissector::field_value_t value;
        EXPECT_EQ(
                dissector.dissect(&value, message, str_args[i].spec,
                        str_args[i].index), ReturnCode::SUCCESS);
        EXPECT_TRUE(value.type == FieldDescriptor::CPPTYPE_STRING);
        EXPECT_STREQ(value.data._str, str_args[i].string_value);
    }
}

TEST_F(ProtobufUtilsTS, caseReflectionFieldInt32)
{
    struct ReflectInt32
    {
        const char * spec;
        int index;
        int32_t value;
    } i32_args[] = { //
            { "bar_number", 0, 11 }, //
            { "bar_foo.number", 0, 22 }, //
            { "bar_foo.numbers", 0, 33 }, //
            { "bar_foo.numbers", 1, 44 }, //
            { "bar_foo.numbers", 2, 55 }, //
            { "bar_foo.numbers", -1, 55 }, //
            { "bar_foo.numbers", -2, 44 }, //
            { "bar_foo.numbers", -3, 33 }, //
            { "bar_numbers", 0, 66 }, //
            { "bar_numbers", 1, 77 }, //
            { "bar_numbers", 2, 88 }, //
            { "bar_numbers", -1, 88 }, //
            { "bar_numbers", -2, 77 }, //
            { "bar_numbers", -3, 66 }, //
            };
    for (size_t i = 0; i < array_size(i32_args); ++i) {
        Dissector::field_value_t value;
        EXPECT_EQ(
                dissector.dissect(&value, message, i32_args[i].spec,
                        i32_args[i].index), ReturnCode::SUCCESS);
        EXPECT_TRUE(value.type == FieldDescriptor::CPPTYPE_INT32);
        EXPECT_EQ(value.data._i32, i32_args[i].value);
    }
}

TEST_F(ProtobufUtilsTS, caseReflectionFieldMessage)
{
    struct ReflectMessage
    {
        const char * spec;
        int index;
        const Message * message;
    } msg_args[] = { //
            { "bar_foos", 0 }, //
            { "bar_foos", 1 }, //
            { "bar_foos", -1 }, //
            { "bar_foos", -2 }, //
            };
    for (size_t i = 0; i < array_size(msg_args); ++i) {
        Dissector::field_value_t value;
        EXPECT_EQ(
                dissector.dissect(&value, message, msg_args[i].spec,
                        msg_args[i].index), ReturnCode::SUCCESS);
        EXPECT_TRUE(value.type == FieldDescriptor::CPPTYPE_MESSAGE);
    }
}
