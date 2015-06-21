/*
 * file name:           src/sample_message.cc
 *
 * author:              Brian Yi ZHANG
 * email:               brianlions@gmail.com
 * date created:        Wed Jun 17 16:17:37 2015 CST
 */

#include <iostream>
#include "foo.pb.h"

// Creates and serializes a Bar message, return byte size, or -1 on error.
int foo_build_and_serialize(void * buf, int size, Bar ** ptr_msg = NULL)
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
    if (ptr_msg) {
        *ptr_msg = a_message;
    }
    else {
        delete a_message;
    }

    return len;
}
