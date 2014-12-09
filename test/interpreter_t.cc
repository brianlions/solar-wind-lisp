/*
 * file name:           test/interpreter_t.cc
 *
 * author:              Brian Yi ZHANG
 * email:               brianlions@gmail.com
 * date created:        Sat Nov 22 23:15:18 2014 CST
 */

#include <gtest/gtest.h>
#include "solarwindlisp.h"

using SolarWindLisp::SimpleInterpreter;
using SolarWindLisp::MatterIF;
using SolarWindLisp::Atom;
using SolarWindLisp::MatterPtr;

#define verbose_print(fmt, ...)                                 \
do {                                                            \
    (void) fprintf(stdout, "[%s:%04d:%s]" fmt "\n",             \
            __FILE__, __LINE__, __FUNCTION__,  ## __VA_ARGS__); \
} while (0)

const long double MATH_PI = 3.141592653;
const long double MATH_E  = 2.718281828;
const long double ROUND_OFF_ERROR = 0.000001;

class InterpreterTS: public testing::Test
{
protected:
    SimpleInterpreter _interpreter;

    void SetUp()
    {
        EXPECT_TRUE(_interpreter.initialize());

        const char * forms[] = { //
            "(define pi 3.141592653)",
            "(define e  2.718281828)",
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

TEST_F(InterpreterTS, caseResultDouble)
{
    /*
     * +,-,* uses i64 by default, / uses long double by default.
     */
    struct triplet_a {
        const char * str;
        Atom::atom_type_t type;
        long double value;
    } items[] = { //
        { "12345",                    Atom::atom_i32,         12345 },
        { "(+ 12 34)",                Atom::atom_i64,         46 },
        { "(- 12 34)",                Atom::atom_i64,         -22 },
        { "(* 5 12)",                 Atom::atom_i64,         60 },
        { "pi",                       Atom::atom_double,      MATH_PI },
        { "e",                        Atom::atom_double,      MATH_E },
        { "(circle-area 3)",          Atom::atom_long_double, MATH_PI * 3 * 3 },
        { "(circle-circumference 5)", Atom::atom_long_double, MATH_PI * 2 * 5 },
        { "(inc e)",                  Atom::atom_long_double, MATH_E + 1 },
        { "(silly-double e)",         Atom::atom_long_double, MATH_E + MATH_E },
        { "(silly-triple pi)",        Atom::atom_long_double, MATH_PI + MATH_PI + MATH_PI },
        { "(silly-x2p1 2.3)",         Atom::atom_long_double, 2.3 + 2.3 + 1 },
        { "(/ 12 34)",                Atom::atom_long_double, 1.0 * 12 / 34 },
        { "(/ 10)",                   Atom::atom_long_double, 0.1 },
        { "(/ pi 5 6)",               Atom::atom_long_double, MATH_PI / (5 * 6) },
    };
    for (size_t i = 0; i < array_size(items); ++i) {
        MatterPtr result = NULL;
        EXPECT_TRUE(result == NULL);
        if (is_verbose()) {
            verbose_print("executing `%s' ...", items[i].str);
        }
        EXPECT_TRUE(_interpreter.execute(result, items[i].str, strlen(items[i].str)));
        EXPECT_TRUE(result != NULL);
        EXPECT_TRUE(result->matter_type() == MatterIF::matter_atom);
        const Atom * expr = static_cast<const Atom *>(result.get());
        EXPECT_EQ(expr->atom_type(), items[i].type);
        long double temp = 0;
        EXPECT_TRUE(expr->to_long_double(temp));
        EXPECT_NEAR(temp, items[i].value, ROUND_OFF_ERROR);
    }
}

TEST_F(InterpreterTS, caseResultProc)
{
    struct pairs {
        const char * str;
        MatterIF::matter_type_t type;
    } items[] = { //
        { "inc",          MatterIF::matter_proc },
        { "silly-double", MatterIF::matter_proc },
        { "silly-triple", MatterIF::matter_proc },
        { "silly-x2p1",   MatterIF::matter_proc },
        { "+",            MatterIF::matter_prim_proc },
        { "-",            MatterIF::matter_prim_proc },
        { "*",            MatterIF::matter_prim_proc },
        { "/",            MatterIF::matter_prim_proc },
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

class FunctionalProgrammingTS: public testing::Test
{
protected:
    SimpleInterpreter _interpreter;

    struct LispTestCases {
        const char * str;
        long double value;
    };

    void SetUp()
    {
        EXPECT_TRUE(_interpreter.initialize());

        const char * essential[] = { //
            "(define pi 3.141592653)",
            "(define e  2.718281828)",

            "(defn   square (v) (* v v))",
            "(define cube (lambda (v) (* v v v)))",

            "(define author-name (lambda () 'Brian Yi ZHANG'))",
            "(defn   author-email () \"brianlions@gmail.com\")",

            "(define bigger"
            "    (lambda (a b)"
            "        (if (> a b) a b)))",
            "(defn   smaller (a b)"
            "    (if (< a b) a b))",
        };

        run_user_forms(essential, array_size(essential));
    }

    void run_user_forms(const char ** forms, size_t count)
    {
        for (size_t i = 0; i < count; ++i) {
            const char * expr = forms[i];
            MatterPtr result = NULL;
            EXPECT_TRUE(result == NULL);
            if (is_verbose()) {
                verbose_print("executing `%s' ...", expr);
            }
            EXPECT_TRUE(_interpreter.execute(result, expr, strlen(forms[i])));
            EXPECT_TRUE(result == NULL);
        }
    }

    void run_lisp_test_cases(const LispTestCases * testcases, size_t count)
    {
        for (size_t i = 0; i < count; ++i) {
            const LispTestCases * one_case = testcases + i;
            MatterPtr result = NULL;
            EXPECT_TRUE(result == NULL);
            if (is_verbose()) {
                verbose_print("executing `%s' ...", one_case->str);
            }
            EXPECT_TRUE(_interpreter.execute(result, one_case->str, strlen(one_case->str)));
            EXPECT_TRUE(result != NULL);
            const Atom * expr = static_cast<const Atom *>(result.get());
            long double temp = 0;
            EXPECT_TRUE(expr->to_long_double(temp));
            EXPECT_EQ(temp, one_case->value);
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

TEST_F(FunctionalProgrammingTS, case_basic)
{
    LispTestCases lisp_test_cases[] = {
        { "(inc 5)",                    6 },
        { "(inc (* 3 5))",              16 },
        { "(dec 10)",                   9 },
        { "(dec (- 2))",                -3 },
        { "(square 5)",                 25 },
        { "(square (+ 3 2))",           25 },
        { "(cube (- 2))",               -8 },
        { "(square (inc 7))",           64 },
        { "(square (bigger 3 5))",      25 },
        { "(square (smaller (inc 9) (dec 20)))",        100 },
    };

    run_lisp_test_cases(lisp_test_cases, array_size(lisp_test_cases));
}

TEST_F(FunctionalProgrammingTS, case_fcompose_and_closure)
{
    const char * forms[] = {
        // multi line expr
        "(defn fcompose (f g)"
        "    (lambda (x) (g (f x))))",
        "(define fcompose2"
        "    (lambda (f g)"
        "        (lambda (x) (g (f x)))))",
        // closure
        "(define mul-by-n"
        "    (lambda (n)"
        "        (lambda (value) (* value n))))",
        "(define mul-by-2 (mul-by-n 2))",
        "(define mul-by-3 (mul-by-n 3))",
        "(defn   add-by-n (n)"
        "    (lambda (value) (+ value n)))",
        "(define add-by-10 (add-by-n 10))",
        "(define add-by-20 (add-by-n 20))",
    };

    LispTestCases lisp_test_cases[] = {
        { "((fcompose inc dec) 10)",    10 },
        { "((fcompose inc square) 10)", (10 + 1) * (10 + 1) },
        { "((fcompose square inc) 5)",  5 * 5 + 1 },
        { "(mul-by-2 10)",              10 * 2 },
        { "(mul-by-3 10)",              10 * 3 },
        { "(add-by-10 100)",            100 + 10 },
        { "(add-by-20 100)",            100 + 20 },
        { "((fcompose mul-by-2 add-by-10) 100)",        100 * 2 + 10 },

    };

    run_user_forms(forms, array_size(forms));
    run_lisp_test_cases(lisp_test_cases, array_size(lisp_test_cases));
}

TEST_F(FunctionalProgrammingTS, case_factorial_and_fibonacci)
{
    const char *forms[] = {
        // recursive - factorial, using define
        "(define factorial"
        "    (lambda (n)"
        "        (if (= n 0)"
        "            1"
        "            (* n (factorial (dec n))))))",

        // recursive - fibonacci, using defn
        "(defn fibonacci (n)"
        "    (if (<= n 2)"
        "        1"
        "        (+ (fibonacci (- n 1))"
        "           (fibonacci (- n 2)))))",
    };

    LispTestCases lisp_test_cases[] = {
        { "(factorial 0)", 1 },
        { "(factorial 1)", 1 },
        { "(factorial 2)", 2 },
        { "(factorial 3)", 6 },
        { "(factorial 4)", 24 },
        { "(factorial 5)", 120 },
        { "(factorial 6)", 720 },
        { "(factorial 7)", 5040 },
        { "(factorial 8)", 40320 },

        { "(fibonacci 1)",   1 },
        { "(fibonacci 2)",   1 },
        { "(fibonacci 3)",   2 },
        { "(fibonacci 4)",   3 },
        { "(fibonacci 5)",   5 },
        { "(fibonacci 6)",   8 },
        { "(fibonacci 7)",  13 },
        { "(fibonacci 8)",  21 },
        { "(fibonacci 9)",  34 },
        { "(fibonacci 10)", 55 },
    };

    run_user_forms(forms, array_size(forms));
    run_lisp_test_cases(lisp_test_cases, array_size(lisp_test_cases));
}

TEST_F(FunctionalProgrammingTS, case_find_max)
{
    const char * forms[] = {
        // recursive
        // apply f(v) for each v in [start,finish], return max of f(v)
        "(defn find-max (f start finish)"
        "    (if (= start finish)"
        "        (f start)"
        "        (bigger (f start)"
        "                (find-max f (+ 1 start) finish))))",
    };

    LispTestCases lisp_test_cases[] = {
        // find max of x, for x in [20,30]
        { "(find-max (lambda (x) x) 20 30)",         30 },
        // find max of (100 - x) for x in [20,30]
        { "(find-max (lambda (x) (- 100 x)) 20 30)", 80 },
        // find max of (- x * x + 6 * x) for x in [0,5]
        { "(find-max (lambda (x) (* x (- 6 x))) 0 5)", 9 },
    };

    run_user_forms(forms, array_size(forms));
    run_lisp_test_cases(lisp_test_cases, array_size(lisp_test_cases));
}

TEST_F(FunctionalProgrammingTS, case_pairs)
{
    const char * forms[] = {
        // pairs
        "(defn slow-cons (a b) (lambda (p) (if p a b)))",
        "(defn slow-car  (pair) (pair true))",
        "(defn slow-cdr  (pair) (pair false))",
        "(define from1to5"
        "    (slow-cons 1"
        "        (slow-cons 2"
        "            (slow-cons 3 (slow-cons 4 5)))))",
    };

    LispTestCases lisp_test_cases[] = {
        { "(slow-car from1to5)", 1 },
        { "(slow-car (slow-cdr from1to5))", 2 },
        { "(slow-car (slow-cdr (slow-cdr from1to5)))", 3 },
    };

    run_user_forms(forms, array_size(forms));
    run_lisp_test_cases(lisp_test_cases, array_size(lisp_test_cases));
}

TEST_F(FunctionalProgrammingTS, case_triple)
{
    const char * forms[] = {
        // creator
        "(defn build-v1 (a b c)"
        "    (lambda (p)"
        "        (if (= p 0)"
        "            a"
        "            (if (= p 1) b c))))",
        // accessor
        "(defn first-v1  (t) (t 0))",
        "(defn second-v1 (t) (t 1))",
        "(defn third-v1  (t) (t 2))",
        // the variable
        "(define t1 (build-v1 1 2 3))",

        // helper funcs
        "(defn slow-cons (a b)  (lambda (p) (if p a b)))",
        "(defn slow-car  (pair) (pair true))",
        "(defn slow-cdr  (pair) (pair false))",
        // creator
        "(defn build-v2 (a b c)"
        "    (slow-cons a (slow-cons b c)))",
        // accessor
        "(defn first-v2  (t) (slow-car t))",
        "(defn second-v2 (t) (slow-car (slow-cdr t)))",
        "(defn third-v2  (t) (slow-cdr (slow-cdr t)))",
        // the variable
        "(define t2 (build-v2 10 20 30))",
    };

    LispTestCases lisp_test_cases[] = {
        { "(first-v1 t1)",  1 },
        { "(second-v1 t1)", 2 },
        { "(third-v1 t1)",  3 },

        { "(first-v2 t2)",  10 },
        { "(second-v2 t2)", 20 },
        { "(third-v2 t2)",  30 },
    };

    run_user_forms(forms, array_size(forms));
    run_lisp_test_cases(lisp_test_cases, array_size(lisp_test_cases));
}

