#include <stdlib.h>
#include <stdio.h>
#include "solarwindlisp.h"
#include "gnu_attributes.h"

#define REPORT(stream, fmt, ...) \
do { \
    char time_str[64]; \
    (void) fprintf(stream, "[%s %s:%04d:%s] " fmt "\n", \
        SolarWindLisp::Utils::Time::hires_ascii_time(time_str, \
                sizeof(time_str)), __FILE__, __LINE__, __FUNCTION__, \
                ## __VA_ARGS__); \
} while (0)

namespace SolarWindLisp
{

class TimedInterpreter: public InterpreterIF
{
public:
    static const uint32_t DEFAULT_RUNS = 10000;
    double timed_expr(MatterPtr &result, const MatterPtr &expr, uint32_t times =
            DEFAULT_RUNS)
    {
        if (!times) {
            return -1.0;
        }

        MatterPtr local_result;
        int64_t start_time = Utils::Time::timestamp_usec();
        for (uint32_t i = 0; i < times; ++i) {
            if (!this->execute_expr(local_result, expr)) {
                return -1.0;
            }
        }
        int64_t finish_time = Utils::Time::timestamp_usec();
        double avg_time_usec = 1.0 * (finish_time - start_time) / times;
        double total_sec UNUSED = 1.0 * (finish_time - start_time) / 1000000;
#if 0
        REPORT(stderr, "[%ld, %ld] %.6f sec, %.6f usec", start_time,
                finish_time, total_sec, avg_time_usec);
#endif
        return avg_time_usec;
    }
};

const uint32_t TimedInterpreter::DEFAULT_RUNS;
}

int main(int argc, char ** argv)
{
    static const uint32_t N_TIMES = 1000 * 1000;
    struct PerfTestCases
    {
        const char * str;
        uint32_t times;
    } items[] = { //
        { "(if 1 2 3)",                 N_TIMES }, //
        { "(if true true false)",       N_TIMES }, //
        { "(if false true false)",      N_TIMES }, //
        { "(if (- 5 3) 1 0)",           N_TIMES }, //
        { "(- 5)",                      N_TIMES }, //
        { "(+ 10)",                     N_TIMES }, //
        { "(+ 123 456)",                N_TIMES }, //
        { "(* 3.141592653 25)",         N_TIMES }, //
        { "(/ 3.141592653 2.718281828)",N_TIMES }, //
        { "3.141592653",                N_TIMES }, //
        { "2.718281828",                N_TIMES }, //
        { "((lambda (r) (* 3.141952653 r r)) 5)", N_TIMES }, //
        { "((lambda (v) (+ v 1)) 10)",  N_TIMES }, //
        { "(and true true)",            N_TIMES }, //
        { "(and true false)",           N_TIMES }, //
        { "(and false true)",           N_TIMES }, //
        { "(and false false)",          N_TIMES }, //
        { "(and (+ 5 3) (- 9 8))",      N_TIMES }, //
        { "(or true true)",             N_TIMES }, //
        { "(or true false)",            N_TIMES }, //
        { "(or false true)",            N_TIMES }, //
        { "(or false false)",           N_TIMES }, //
        { "(or (- 10 7 3) true)",       N_TIMES }, //
        { "(+ (- 6) (* 2 3))",          N_TIMES }, //
        { "(or (- 10 8 2) (+ (- 6) (* 2 3)))", N_TIMES }, //
        { "(or (- 10 5 2) (+ (- 6) (* 2 3)))", N_TIMES }, //
        { "(+ 100 200 300 400 500)",    N_TIMES }, //
        { "(* 11 13 17 19 23 29)",      N_TIMES }, //
    };

    SolarWindLisp::SimpleParser simple_parser;
    REPORT(stderr, "===== every expr will be execute for %u times =====", N_TIMES);
    for (size_t idx = 0; idx < array_size(items); ++idx) {
        SolarWindLisp::MatterPtr expr = simple_parser.parse(
                items[idx].str, strlen(items[idx].str));
        if (!expr || expr->matter_type() !=
                SolarWindLisp::MatterIF::matter_composite_expr) {
            REPORT(stderr, "failed parsing expr `%s'",
                    items[idx].str);
            continue;
        }

        SolarWindLisp::TimedInterpreter interp;
        if (!interp.initialize()) {
            continue;
        }

        SolarWindLisp::MatterPtr result = NULL;
        SolarWindLisp::CompositeExpr * ce =
            static_cast<SolarWindLisp::CompositeExpr *>(expr.get());
        //REPORT(stderr, "executing expr `%s' %u times ...", items[idx].str, items[idx].times);
        double avg_usec = interp.timed_expr(result, ce->get(0), items[idx].times);
        if (avg_usec < 0) {
            REPORT(stderr, "something is wrong!");
        }
        else {
            //REPORT(stderr, "avg time cost is %0.6f usec", avg_usec);
            REPORT(stderr, "%12.6f usec\t\texpr `%s'", avg_usec, items[idx].str);
        }
    }

    exit (EXIT_SUCCESS);
}
