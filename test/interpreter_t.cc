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
    const char * forms[] = { "12345" };
    SimpleInterpreter interpreter;
    EXPECT_TRUE(interpreter.initialize());
    IMatter * result = NULL;
    for (size_t i = 0; i < array_size(forms); ++i) {
        EXPECT_TRUE(interpreter.execute(forms[i], strlen(forms[i]), &result));
    }
    EXPECT_TRUE(result != NULL);
    EXPECT_TRUE(result->is_atom());

    if (is_verbose()) {
        std::cout << "result: " << result->debug_string(is_compact(), 0) << std::endl;
    }
}
