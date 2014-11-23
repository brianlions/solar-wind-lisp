#include <gtest/gtest.h>
#include "solarwindlisp.h"

using SolarWindLisp::SimpleInterpreter;
using SolarWindLisp::IMatter;

class InterpreterTS: public testing::Test
{
protected:
    void SetUp()
    {
    }

    void TearDown()
    {
    }

    bool is_verbose() const
    {
        return true;
    }

    bool is_compact() const
    {
        return false;
    }
};

TEST_F(InterpreterTS, caseExecuteA)
{
    const char * forms[] = { //
        "12345",
        "(+ 12 34)",
        "(- 12 34)",
        "(* 12 34)",
        "(/ 12 34)",
        "(mod 34 12)",
    };
    SimpleInterpreter interpreter;
    EXPECT_TRUE(interpreter.initialize());
    for (size_t i = 0; i < array_size(forms); ++i) {
        IMatter * result = NULL;
        EXPECT_TRUE(interpreter.execute(&result, forms[i], strlen(forms[i])));
        EXPECT_TRUE(result != NULL);
        if (!result) {
            continue;
        }

        EXPECT_TRUE(result->is_atom());
        if (is_verbose()) {
            std::cout
                << "result: "
                << result->debug_string(is_compact())
                << std::endl;
        }
    }
}
