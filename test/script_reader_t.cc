/*
 * file name:           test/script_reader_t.cc
 *
 * author:              Brian Yi ZHANG
 * email:               brianlions@gmail.com
 * date created:        Wed Dec 03 22:12:29 2014 CST
 */

#include <unistd.h>
#include <stdio.h>
#include <gtest/gtest.h>
#include "script_reader.h"
#include "utils.h"

using SolarWindLisp::ScriptFileReader;

class ScriptFileReaderTS: public testing::Test
{
protected:
    static const char * _script_file_name;
    char * _stmt_buf;
    ssize_t _stmt_buf_size;
    ScriptFileReader _sfr;

    virtual void SetUp()
    {
        _stmt_buf = (char *) malloc(1024);
        EXPECT_TRUE(_stmt_buf != NULL);
        _stmt_buf_size = 1024;
    }

    virtual void TearDown()
    {
        free(_stmt_buf);
        _stmt_buf = NULL;
        _stmt_buf_size = 0;
        unlink(_script_file_name);
    }

    void save_to_file(const char * data)
    {
        FILE * fh = fopen(_script_file_name, "w+");
        EXPECT_TRUE(fh != NULL);
        if (strlen(data) > 0) {
            EXPECT_TRUE(fprintf(fh, "%s", data) > 0);
        }
        fclose(fh);
    }
};

const char * ScriptFileReaderTS::_script_file_name =
        "swl_ScriptFileReaderTestSuite.swl";

TEST_F(ScriptFileReaderTS, case01)
{
    save_to_file( //
            "(define pi 3.14159)" //
            //"pi\n" //
            "(define e 2.71828)" //
            //"e\n" //
            "\n(define x2 (lambda (r) (* r 2)))" //
            "\n(define inc (lambda (v) (+ v 1)))" //

            // a multi line expr
            "(when (= 1 (- 9 8))\n"
            "(- 9 3)\n"
            "(* 4 7))"

            // another multi line expr
            "(defn one-two-other (n)\n"
            "    (cond\n"
            "        (= n 1) 100\n"
            "        (= n 2) 200\n"
            "        :default 300))"
    );

    const char * forms[] = { //
        "(define pi 3.14159)", //
        "(define e 2.71828)", //
        "\n(define x2 (lambda (r) (* r 2)))", //
        "\n(define inc (lambda (v) (+ v 1)))", //
        // multi line expr
        "(when (= 1 (- 9 8))\n"
        "(- 9 3)\n"
        "(* 4 7))",
        // leading white-space char removed
        "(defn one-two-other (n)\n"
        "(cond\n"
        "(= n 1) 100\n"
        "(= n 2) 200\n"
        ":default 300))"
    };
    EXPECT_TRUE(_sfr.initialize(_script_file_name));
    for (size_t i = 0; i < array_size(forms); ++i) {
        EXPECT_TRUE(_sfr.get_next_statement(&_stmt_buf, &_stmt_buf_size) > 0);
        EXPECT_STREQ(forms[i], _stmt_buf);
    }
}
