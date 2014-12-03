#include <gtest/gtest.h>
#include "solarwindlisp.h"

using SolarWindLisp::SimpleInterpreter;
using SolarWindLisp::IMatter;
using SolarWindLisp::Expr;
using SolarWindLisp::MatterPtr;

#define verbose_print(fmt, ...)                                 \
do {                                                            \
    (void) fprintf(stdout, "[%s:%04d:%s]" fmt "\n",             \
            __FILE__, __LINE__, __FUNCTION__,  ## __VA_ARGS__); \
} while (0)

class InterpreterTS: public testing::Test
{
protected:
    SimpleInterpreter _interpreter;
    static const long double _pi;
    static const long double _e;
    static const long double _absolute_error;

    void SetUp()
    {
        EXPECT_TRUE(_interpreter.initialize());

        const char * forms[] = { //
            "(define pi 3.141592653)",
            "(define e  2.718281828)",
            "(define inc (lambda (v) (+ v 1)))",
            "(define silly-double (lambda (v) (+ v v)))",
            "(define silly-triple (lambda (v) (+ v (+ v v))))",
            "(define silly-x2p1 (lambda (v) (+ v 1 v)))",
            "(define circle-area (lambda (r) (* pi r r)))",
            "(define circle-circumference (lambda (r) (* 2 pi r)))",
        };
        for (size_t i = 0; i < array_size(forms); ++i) {
            MatterPtr result = NULL;
            EXPECT_TRUE(result == NULL);
            if (is_verbose()) {
                verbose_print("executing `%s' ...", forms[i]);
            }
            EXPECT_TRUE(_interpreter.execute(result, forms[i], strlen(forms[i])));
            EXPECT_TRUE(result == NULL);
        }
    }

    void TearDown()
    {
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

const long double InterpreterTS::_pi = 3.141592653;
const long double InterpreterTS::_e  = 2.718281828;
const long double InterpreterTS::_absolute_error = 0.000001;

TEST_F(InterpreterTS, caseResultDouble)
{
    /*
     * +,-,* uses i64 by default, / uses long double by default.
     */
    struct triplet_a {
        const char * str;
        Expr::atom_type_t type;
        long double value;
    } items[] = { //
        { "12345",                    Expr::atom_i32,         12345 },
        { "(+ 12 34)",                Expr::atom_i64,         46 },
        { "(- 12 34)",                Expr::atom_i64,         -22 },
        { "(* 5 12)",                 Expr::atom_i64,         60 },
        { "pi",                       Expr::atom_double,      _pi },
        { "e",                        Expr::atom_double,      _e },
        { "(circle-area 3)",          Expr::atom_long_double, _pi * 3 * 3 },
        { "(circle-circumference 5)", Expr::atom_long_double, _pi * 2 * 5 },
        { "(inc e)",                  Expr::atom_long_double, _e + 1 },
        { "(silly-double e)",         Expr::atom_long_double, _e + _e },
        { "(silly-triple pi)",        Expr::atom_long_double, _pi + _pi + _pi },
        { "(silly-x2p1 2.3)",         Expr::atom_long_double, 2.3 + 2.3 + 1 },
        { "(/ 12 34)",                Expr::atom_long_double, 1.0 * 12 / 34 },
        { "(/ 10)",                   Expr::atom_long_double, 0.1 },
        { "(/ pi 5 6)",               Expr::atom_long_double, _pi / (5 * 6) },
    };
    for (size_t i = 0; i < array_size(items); ++i) {
        MatterPtr result = NULL;
        EXPECT_TRUE(result == NULL);
        if (is_verbose()) {
            verbose_print("executing `%s' ...", items[i].str);
        }
        EXPECT_TRUE(_interpreter.execute(result, items[i].str, strlen(items[i].str)));
        EXPECT_TRUE(result != NULL);
        EXPECT_TRUE(result->matter_type() == IMatter::matter_atom);
        const Expr * expr = static_cast<const Expr *>(result.get());
        EXPECT_EQ(expr->atom_type(), items[i].type);
        long double temp = 0;
        EXPECT_TRUE(expr->to_long_double(temp));
        EXPECT_NEAR(temp, items[i].value, _absolute_error);
    }
}

TEST_F(InterpreterTS, caseResultProc)
{
    struct pairs {
        const char * str;
        IMatter::matter_type_t type;
    } items[] = { //
        { "inc",          IMatter::matter_proc },
        { "silly-double", IMatter::matter_proc },
        { "silly-triple", IMatter::matter_proc },
        { "silly-x2p1",   IMatter::matter_proc },
        { "+",            IMatter::matter_prim_proc },
        { "-",            IMatter::matter_prim_proc },
        { "*",            IMatter::matter_prim_proc },
        { "/",            IMatter::matter_prim_proc },
    };
    for (size_t i = 0; i < array_size(items); ++i) {
        MatterPtr result = NULL;
        EXPECT_TRUE(result == NULL);
        if (is_verbose()) {
            verbose_print("executing `%s' ...", items[i].str);
        }
        EXPECT_TRUE(_interpreter.execute(result, items[i].str, strlen(items[i].str)));
        EXPECT_TRUE(result != NULL);
        EXPECT_TRUE(result->matter_type() == items[i].type);
    }
}
