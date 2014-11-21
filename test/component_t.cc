#include <gtest/gtest.h>
#include "swl_component.h"
#include "swl_expr.h"
#include "swl_utils.h"

using SolarWindLisp::IExpr;
using SolarWindLisp::Expr;
using SolarWindLisp::CompositeExpr;

class ExprTS: public testing::Test
{
protected:
    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }

    bool is_verbose() const
    {
        return false;
    }

    bool is_compact() const
    {
        return true;
    }
};

TEST_F(ExprTS, types)
{
    Expr e;
    EXPECT_EQ(e.type(), IExpr::type_none);
    EXPECT_EQ(e.is_none(), true);
}

TEST_F(ExprTS, parseA)
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
        { "this is a string", IExpr::type_string },
        { "", IExpr::type_string },
    };

    for (size_t i = 0; i < array_size(good_cases); ++i)
    {
        EXPECT_TRUE(e.parse(good_cases[i].input, strlen(good_cases[i].input)));
        EXPECT_EQ(e.type(), good_cases[i].type);
        if (is_verbose()) {
            std::cout << "expr # " << i << ": " << e.debug_string(is_compact(), 0) << std::endl;
        }
    }
}

TEST_F(ExprTS, parseA1)
{
    Expr e;

    EXPECT_FALSE(e.parse_none("", 0));
    EXPECT_FALSE(e.parse_none("none ", 5));
    EXPECT_FALSE(e.parse_none("none!!", 6));
    EXPECT_FALSE(e.parse_none("true.", 4));
    EXPECT_FALSE(e.parse_none("true!!", 6));
    EXPECT_FALSE(e.parse_none("true", 4));
    EXPECT_TRUE(e.parse_none("none", 4));

    EXPECT_FALSE(e.parse_bool("", 0));
    EXPECT_TRUE(e.parse_bool("true", 4));
    EXPECT_TRUE(e.parse_bool("false", 5));
    EXPECT_FALSE(e.parse_bool("true!", 5));
    EXPECT_FALSE(e.parse_bool("TrUe", 4));

    EXPECT_FALSE(e.parse_integer("", 0));
    EXPECT_FALSE(e.parse_real("", 0));
    EXPECT_TRUE(e.parse_string("", 0));
}

TEST_F(ExprTS, parseB)
{
    int32_t i32 = 0;
    uint32_t u32 = 0;
    int64_t i64 = 0;
    uint64_t u64 = 0;

    Expr e;

    struct tuple {
        const char * input;
        int32_t i32;
        uint32_t u32;
        int64_t i64;
        uint64_t u64;
    } good_cases[] = {
            { "none", 0, 0U, 0, 0UL },
            { "false", 0, 0U, 0, 0UL },
            { "true", 1, 1U, 1, 1UL },
            { "123",  123, 123U, 123, 123UL },
    };

    for (size_t i = 0; i < array_size(good_cases); ++i) {
        EXPECT_TRUE(e.parse(good_cases[i].input, strlen(good_cases[i].input)));
        EXPECT_TRUE(e.to_i32(i32) && i32 == good_cases[i].i32);
        EXPECT_TRUE(e.to_u32(u32) && u32 == good_cases[i].u32);
        EXPECT_TRUE(e.to_i64(i64) && i64 == good_cases[i].i64);
        EXPECT_TRUE(e.to_u64(u64) && u64 == good_cases[i].u64);
    }
}

TEST_F(ExprTS, parseC)
{
    CompositeExpr ce;
    const char * expr_list[] = {
            "none",
            "true",
            "false",
            "12345",
            "0xabcd",
            "a string",
            "another string",
    };

    for (size_t i = 0; i < array_size(expr_list); ++i) {
        Expr * e = Expr::create(expr_list[i], strlen(expr_list[i]));
        EXPECT_TRUE(e != NULL);
        EXPECT_TRUE(ce.append_expr(e));
    }

    if (is_verbose()) {
        std::cout << ce.debug_string(is_compact()) << std::endl;
    }

    EXPECT_EQ(ce.size(), 7);
    EXPECT_TRUE(ce.rewind());
    size_t counter = 0;
    int32_t i32 = 0;
    uint32_t u32 = 0;
    while (ce.has_next())
    {
        const IExpr * temp = ce.get_next();
        EXPECT_TRUE(temp != NULL);
        EXPECT_FALSE(temp->is_composite());

        const Expr * e = static_cast<const Expr *>(temp);
        EXPECT_TRUE(e != NULL);
        switch (counter++)
        {
            case 0:
                EXPECT_TRUE(e->is_none());
                EXPECT_TRUE(e->to_i32(i32) && i32 == 0);
                break;
            case 1:
                EXPECT_TRUE(e->is_bool());
                EXPECT_TRUE(e->to_i32(i32) && i32 == 1);
                break;
            case 2:
                EXPECT_TRUE(e->is_bool());
                EXPECT_TRUE(e->to_i32(i32) && i32 == 0);
                break;
            case 3:
                EXPECT_TRUE(e->is_i32());
                EXPECT_TRUE(e->to_i32(i32) && i32 == 12345);
                break;
            case 4:
                EXPECT_TRUE(e->is_u32());
                EXPECT_TRUE(e->to_u32(u32) && u32 == 0xabcd);
                break;
            case 5:
                EXPECT_TRUE(e->is_string());
                EXPECT_STREQ(e->to_string(), "a string");
                break;
            case 6:
                EXPECT_TRUE(e->is_string());
                EXPECT_STREQ(e->to_string(), "another string");
                break;
            default:
                break;
        }
    }
}
