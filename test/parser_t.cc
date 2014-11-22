#include <iostream>
#include <gtest/gtest.h>
#include "swl_expr_if.h"
#include "swl_expr.h"
#include "swl_parser.h"
#include "swl_utils.h"

using SolarWindLisp::IExpr;
using SolarWindLisp::Expr;
using SolarWindLisp::CompositeExpr;
using SolarWindLisp::IParser;
using SolarWindLisp::SimpleParser;

class ParserTokenizeTS: public testing::Test
{
protected:
    IParser::lexical_tokens _tokens;
    SimpleParser _parser;

    virtual void SetUp()
    {
        _tokens.clear();
    }

    virtual void TearDown()
    {
        _tokens.clear();
    }

    bool is_verbose() const
    {
        return true;
    }

    bool is_compact() const
    {
        return false;
    }

    void parse_and_compare(
            const char * filename, size_t linenum,
            const char * form_str, const char ** strings,
            size_t n_str)
    {
        if (is_verbose()) {
            std::cout << "[running case] file: " << filename << ", line: " << linenum << std::endl;
        }
        // without the 3rd parameter `length'
        EXPECT_TRUE(_parser.tokenize(&_tokens, form_str));
        EXPECT_EQ(_tokens.size(), n_str);
        for (size_t i = 0; i < n_str; ++i)
        {
            if (is_verbose()) {
                std::cout
                    << "_tokens[" << i << "]: `" << _tokens[i] << "'" << std::endl
                    << "strings[" << i << "]: `" << strings[i] << "'" << std::endl;
            }
            EXPECT_TRUE(_tokens[i] == strings[i]);
        }

        // with the 3rd parameter `length'
        EXPECT_TRUE(_parser.tokenize(&_tokens, form_str, strlen(form_str)));
        EXPECT_EQ(_tokens.size(), n_str);
        for (size_t i = 0; i < n_str; ++i)
        {
            EXPECT_TRUE(_tokens[i] == strings[i]);
        }
    }
};

TEST_F(ParserTokenizeTS, testA01)
{
    const char * form_str = "(define sum (lambda (a b) (+ a b)))";
    const char * cstr_tokens[] =
    { "(", "define", "sum", "(", "lambda", "(", "a", "b", ")", "(", "+", "a",
            "b", ")", ")", ")" };
    parse_and_compare(__FILE__, __LINE__, form_str, cstr_tokens, array_size(cstr_tokens));
}

TEST_F(ParserTokenizeTS, testA02)
{
    const char * form_str = "(defn max (a b) (if (> a b) a b))";
    const char * cstr_tokens[] =
    { "(", "defn", "max", "(", "a", "b", ")", "(", "if", "(", ">", "a", "b",
            ")", "a", "b", ")", ")" };
    parse_and_compare(__FILE__, __LINE__, form_str, cstr_tokens, array_size(cstr_tokens));
}

TEST_F(ParserTokenizeTS, testA03)
{
    const char * form_str = "(defn author-email () \"zhangyi21@baidu.com\")";
    const char * cstr_tokens[] =
    { "(", "defn", "author-email", "(", ")", "\"zhangyi21@baidu.com\"", ")"};
    parse_and_compare(__FILE__, __LINE__, form_str, cstr_tokens, array_size(cstr_tokens));
}

TEST_F(ParserTokenizeTS, testA04)
{
    // string passed to parser:
    //     (println "escaped quote \" and \', bingo!")
    // from parser's perspective, the string after println is:
    //              "escaped quote " and ', bingo!"
    const char * form_str = "(println \"escaped quote \\\" and \\', bingo!\")";
    const char * cstr_tokens[] =
    { "(", "println", "\"escaped quote \" and ', bingo!\"", ")" };
    parse_and_compare(__FILE__, __LINE__, form_str, cstr_tokens, array_size(cstr_tokens));
}

TEST_F(ParserTokenizeTS, parseExpr)
{
    struct tuple {
        const char * forms;
        size_t n_expr;
    } cases[] = {
        { "(define sum (lambda (a b) (+ a b)))", 1 },
        { "(defn max (a b) (if (> a b) a b))", 1 },
        { "(defn author-email () \"zhangyi21@baidu.com\")", 1 },
        { "(println \"escaped quote \\\" and \\', bingo!\")", 1 },
        { "3.141592653", 1 },
        { "(define pi 3.141592653) (define two-pi (* 2 pi))", 2 },
    };

    for (size_t i = 0; i < array_size(cases); ++i)
    {
        IExpr * e = _parser.parse(cases[i].forms, strlen(cases[i].forms));
        EXPECT_TRUE(e != NULL);
        EXPECT_EQ(e->is_atom(), false);
        EXPECT_EQ(e->is_molecule(), true);
        EXPECT_EQ((static_cast<CompositeExpr *>(e))->size(), cases[i].n_expr);
        if (is_verbose()) {
            std::cout
                << "form: `" << cases[i].forms << "'" << std::endl
                << "IExpr: " << e->debug_string(is_compact()) << std::endl;
        }
        delete e;
    }
}
