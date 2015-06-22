/*
 * file name:           src/sample_message.cc
 *
 * author:              Brian Yi ZHANG
 * email:               brianlions@gmail.com
 * date created:        Wed Jun 17 16:17:37 2015 CST
 */

#include <stdio.h>
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

#if 0
    std::cout
            << std::endl //
            << "----------------------------------------" << std::endl
            << "protobuf message is: " << std::endl //
            << a_message->DebugString() << std::endl //
            << "----------------------------------------" << std::endl;
#endif
    if (ptr_msg) {
        *ptr_msg = a_message;
    }
    else {
        delete a_message;
    }

    return len;
}

//----------------------------------------------------------

static void fill_person(Person * person, int32_t value)
{
    char buf[32];
    snprintf(buf, sizeof(buf), "person_%04d", value);
    person->set_name(buf);
    person->set_gender(value % 2 == 0 ? FEMALE : MALE);
    person->set_age(20 + (value % 50));
    person->set_height(170 + (value % 10) - 5);
    person->set_weight(60 + (value % 10) - 5);
    snprintf(buf, sizeof(buf), "address_%04d", value);
    person->set_address(buf);
    snprintf(buf, sizeof(buf), "seat_%04d", value);
    person->add_lucky_numbers(value % 10 + 1);
    person->add_lucky_numbers(value % 10 + 2);
    person->add_lucky_numbers(value % 10 + 3);
}

static void fill_department(Department * department, int32_t prsn_id,
        int32_t dep_size)
{
    char buf[32];
    snprintf(buf, sizeof(buf), "department_%04d", prsn_id);
    department->set_name(buf);
    snprintf(buf, sizeof(buf), "department_desc_%04d", prsn_id);
    department->set_description(buf);
    department->set_department_id(prsn_id);
    fill_person(department->mutable_manager(), prsn_id + 1);
    fill_person(department->add_deputy_managers(), prsn_id + 2);
    fill_person(department->add_deputy_managers(), prsn_id + 3);
    for (int32_t v = prsn_id + 4; v <= prsn_id + dep_size; ++v) {
        fill_person(department->add_workers(), v);
    }
}

int company_build_and_serialize(Company ** ptr_msg, void * &buf, int &size)
{
    static const int32_t NUM_PRES = 5;
    static const int32_t NUM_DEP  = 10;
    static const int32_t DEP_SIZE = 10;

    Company * company = new Company();
    // fill the company
    company->set_name("TheGreatCompany Ltd.");
    company->set_description("This is a great company!");
    company->set_company_id(1);
    fill_person(company->mutable_president(), 1);
    for (int32_t i = 2; i <= NUM_PRES; ++i) {
        fill_person(company->add_vice_presidents(), i);
    }
    // fill headquarter and departments
    fill_department(company->mutable_headquarter(), 10000, DEP_SIZE);
    for (int32_t i = 2; i <= NUM_DEP; ++i) {
        fill_department(company->add_departments(), i * 10000, DEP_SIZE);
    }

    int len = company->ByteSize();
    if (!buf) {
        if (!(buf = malloc(len))) {
            return -1;
        }
        size = len;
        if (!company->SerializeToArray(buf, size)) {
            free(buf);
            buf = NULL;
            return -1;
        }
    }
    else if (size < len || !company->SerializeToArray(buf, size)) {
        return -1;
    }
    else {
        size = len;
    }

    if (ptr_msg) {
        *ptr_msg = company;
    }
    else {
        delete company;
    }

#if 0
    std::cout
            << std::endl //
            << "----------------------------------------" << std::endl
            << "Company message is: " << std::endl //
            << company->DebugString() << std::endl //
            << "----------------------------------------" << std::endl;
#endif

    return len;
}
