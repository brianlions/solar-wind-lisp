/*
 * file name:           src/pbhrepl_main.cc
 *
 * author:              Brian Yi ZHANG
 * email:               brianlions@gmail.com
 * date created:        Mon Jun 15 16:58:35 2015 CST
 */

#include <stdlib.h>
#include <iostream>

#include "solarwindlisp.h"
#include "protobuf_heavy_interp.h"
#include "foo.pb.h"

// Creates and serializes a Bar message, return byte size, or -1 on error.
int build_and_serialize(void * buf, int size)
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

    int len = a_message->ByteSize();
    if (size < len || !a_message->SerializeToArray(buf, size)) {
        delete a_message;
        return -1;
    }

    std::cout
            << std::endl //
            << "----------------------------------------" << std::endl
            << "protobuf message is: " << std::endl //
            << a_message->DebugString() << std::endl //
            << "----------------------------------------" << std::endl;

    return len;
}

int main(int argc, char ** argv)
{
    char buffer[1024 * 16];
    int len = 0;
    if ((len = build_and_serialize(buffer, sizeof(buffer))) < 0) {
        exit(EXIT_FAILURE);
    }

    SolarWindLisp::ProtobufHeavyInterpreter pbh_inter;
    if (pbh_inter.pre_initialization(buffer, len, "./proto/foo.proto", "Bar")
            != 0) {
        exit(EXIT_FAILURE);
    }

    if (!pbh_inter.initialize()) {
        exit(EXIT_FAILURE);
    }
    pbh_inter.interactive(argc > 1 ? argv[1] : NULL);
    exit(EXIT_SUCCESS);
}
