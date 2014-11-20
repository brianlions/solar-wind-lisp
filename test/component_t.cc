#include <gtest/gtest.h>
#include "swl_component.h"
#include "swl_expr.h"

#ifndef array_size
#define array_size(a) (sizeof(a) / sizeof(a[0]))
#endif

using SolarWindLisp::IExpr;
using SolarWindLisp::Expr;
using SolarWindLisp::CompositeExpr;

class ExprTS: public testing::Test
{
public:
    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }
};

TEST_F(ExprTS, types)
{
    Expr e;
    EXPECT_EQ(e.type(), IExpr::type_none);
    EXPECT_EQ(e.is_none(), true);
}

TEST_F(ExprTS, parse)
{
    Expr e;

    struct tuple
    {
        const char * input;
        IExpr::expr_type type;
    };

    struct tuple good_cases[] =
    {
        { "none", IExpr::type_none },
        { "true", IExpr::type_bool },
        { "false", IExpr::type_bool },
        { "12345", IExpr::type_i32 },
        { "012345", IExpr::type_u32 },
        { "0xabcdef", IExpr::type_u32 },
        { "3.141592653", IExpr::type_double },
        { "2.718281828", IExpr::type_double },
    };

    for (size_t i = 0; i < array_size(good_cases); ++i)
    {
        EXPECT_TRUE(e.parse(good_cases[i].input, strlen(good_cases[i].input)));
        EXPECT_EQ(e.type(), good_cases[i].type);
    }

    const char * bad_cases[] =
    {
        "oh my god!",
        "go to hell!",
        "01237890abc",
        "123   ",
        "0456  ",
        "0x789 ",
        "3.14  ",
        "2.718\t",
    };

    for (size_t i = 0; i < array_size(bad_cases); ++i)
    {
        EXPECT_FALSE(e.parse(bad_cases[i], strlen(bad_cases[i])));
    }
}
