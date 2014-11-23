#include <gtest/gtest.h>
#include "solarwindlisp.h"

using SolarWindLisp::IExpr;
using SolarWindLisp::Expr;
using SolarWindLisp::CompositeExpr;
using SolarWindLisp::SimpleMatterFactory;

class ExprTS: public testing::Test
{
protected:
    SimpleMatterFactory _matter_factory;
    Expr * _expr;
    CompositeExpr * _composite_expr;

    virtual void SetUp()
    {
        _expr = _matter_factory.create_atom();
        _composite_expr = _matter_factory.create_molecule();
        EXPECT_TRUE(_expr != NULL);
        EXPECT_TRUE(_composite_expr != NULL);
    }

    virtual void TearDown()
    {
        EXPECT_TRUE(_expr != NULL);
        EXPECT_TRUE(_composite_expr != NULL);
        if (_expr) {
            delete _expr;
            _expr = NULL;
        }

        if (_composite_expr) {
            delete _composite_expr;
            _composite_expr = NULL;
        }
    }

    bool is_verbose() const
    {
        return false;
    }

    bool is_compact() const
    {
        return false;
    }
};

TEST_F(ExprTS, types)
{
    EXPECT_EQ(_expr->atom_type(), Expr::atom_bool);
    EXPECT_EQ(_expr->is_bool(), true);
}

TEST_F(ExprTS, parseA)
{
    struct tuple
    {
        const char * input;
        Expr::atom_type_t atom_type;
    };

    struct tuple good_cases[] = { //
            { "true", Expr::atom_bool }, //
            { "false", Expr::atom_bool }, //
            { "12345", Expr::atom_i32 }, //
            { "012345", Expr::atom_u32 }, //
            { "0xabcdef", Expr::atom_u32 }, //
            { "3.141592653", Expr::atom_double }, //
            { "2.718281828", Expr::atom_double }, //
            { "this is a string", Expr::atom_cstr }, //
            { "", Expr::atom_cstr }, //
            };

    for (size_t i = 0; i < array_size(good_cases); ++i) {
        EXPECT_TRUE(
                _expr->parse(good_cases[i].input, strlen(good_cases[i].input)));
        EXPECT_EQ(_expr->atom_type(), good_cases[i].atom_type);
        if (is_verbose()) {
            std::cout << "expr # " << i << ": "
                    << _expr->debug_string(is_compact(), 0) << std::endl;
        }
    }
}

TEST_F(ExprTS, parseA1)
{
    EXPECT_FALSE(_expr->parse_bool("", 0));
    EXPECT_TRUE(_expr->parse_bool("true", 4));
    EXPECT_TRUE(_expr->parse_bool("false", 5));
    EXPECT_FALSE(_expr->parse_bool("true!", 5));
    EXPECT_FALSE(_expr->parse_bool("TrUe", 4));

    EXPECT_FALSE(_expr->parse_integer("", 0));
    EXPECT_FALSE(_expr->parse_real("", 0));
    EXPECT_TRUE(_expr->parse_cstr("", 0));
}

TEST_F(ExprTS, parseB)
{
    int32_t i32 = 0;
    uint32_t u32 = 0;
    int64_t i64 = 0;
    uint64_t u64 = 0;

    struct tuple
    {
        const char * input;
        int32_t i32;
        uint32_t u32;
        int64_t i64;
        uint64_t u64;
    } good_cases[] = { //
            { "false", 0, 0U, 0, 0UL }, //
            { "true", 1, 1U, 1, 1UL }, //
            { "123", 123, 123U, 123, 123UL }, //
            };

    for (size_t i = 0; i < array_size(good_cases); ++i) {
        EXPECT_TRUE(
                _expr->parse(good_cases[i].input, strlen(good_cases[i].input)));
        EXPECT_TRUE(_expr->to_i32(i32) && i32 == good_cases[i].i32);
        EXPECT_TRUE(_expr->to_u32(u32) && u32 == good_cases[i].u32);
        EXPECT_TRUE(_expr->to_i64(i64) && i64 == good_cases[i].i64);
        EXPECT_TRUE(_expr->to_u64(u64) && u64 == good_cases[i].u64);
    }
}

TEST_F(ExprTS, parseC)
{
    const char * expr_list[] = { //
            "", //
            "true", //
            "false", //
            "12345", //
            "0xabcd", //
            "a string", //
            "another string", //
            "\'single quote str\'", //
            "\"double quote str\"",
            };
    for (size_t i = 0; i < array_size(expr_list); ++i) {
        Expr * e = Expr::create(expr_list[i], strlen(expr_list[i]));
        EXPECT_TRUE(e != NULL);
        EXPECT_TRUE(_composite_expr->append_expr(e));
    }

    if (is_verbose()) {
        std::cout << _composite_expr->debug_string(is_compact()) << std::endl;
    }

    EXPECT_EQ(_composite_expr->size(), array_size(expr_list));
    EXPECT_TRUE(_composite_expr->rewind());
    size_t counter = 0;
    int32_t i32 = 0;
    uint32_t u32 = 0;
    while (_composite_expr->has_next()) {
        const IExpr * temp = _composite_expr->get_next();
        EXPECT_TRUE(temp != NULL);
        EXPECT_TRUE(temp->is_atom());
        EXPECT_FALSE(temp->is_molecule());

        const Expr * e = static_cast<const Expr *>(temp);
        EXPECT_TRUE(e != NULL);
        switch (counter++) {
            case 0:
                EXPECT_TRUE(e->is_cstr());
                EXPECT_STREQ(e->to_cstr(), "");
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
                EXPECT_TRUE(e->is_cstr());
                EXPECT_STREQ(e->to_cstr(), "a string");
                break;
            case 6:
                EXPECT_TRUE(e->is_cstr());
                EXPECT_STREQ(e->to_cstr(), "another string");
                break;
            case 7:
                EXPECT_TRUE(e->is_cstr());
                EXPECT_TRUE(e->is_quoted_cstr());
                EXPECT_STREQ(e->to_cstr(), "single quote str");
                break;
            case 8:
                EXPECT_TRUE(e->is_cstr());
                EXPECT_TRUE(e->is_quoted_cstr());
                EXPECT_STREQ(e->to_cstr(), "double quote str");
                break;
            default:
                break;
        }
    }
}
