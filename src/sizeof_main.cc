#include <stdio.h>
#include <stdint.h>
#include "release.h"
#include "swl_expr.h"
#include "swl_parser.h"
#include "swl_future.h"
#include "swl_scoped_env.h"
#include "swl_proc.h"
#include "swl_prim_proc.h"
#include "swl_interpreter.h"

void print_header()
{
    printf("  size  type name\n");
    printf("------  -----------\n");
}

#define print_sizeof(v)                         \
do {                                            \
    printf("%6lu\tsizeof(%s)\n", sizeof(v), #v);\
} while (0)

int main(int argc, char ** argv)
{
    print_header();

    print_sizeof(char);
    print_sizeof(short);
    print_sizeof(int);
    print_sizeof(long);
    print_sizeof(long long);
    print_sizeof(float);
    print_sizeof(double);
    print_sizeof(long double);
    print_sizeof(void *);
    printf("\n");
    print_sizeof(SolarWindLisp::IMatter);
    print_sizeof(SolarWindLisp::IMatter::matter_type_t);
    print_sizeof(SolarWindLisp::Expr);
    print_sizeof(SolarWindLisp::Expr::atom_type_t);
    print_sizeof(SolarWindLisp::Expr::AtomData);
    print_sizeof(SolarWindLisp::CompositeExpr);
    print_sizeof(SolarWindLisp::IPrimProc);
    print_sizeof(SolarWindLisp::Proc);
    print_sizeof(SolarWindLisp::Future);
    print_sizeof(SolarWindLisp::ScopedEnv);
    print_sizeof(SolarWindLisp::IMatterFactory);
    print_sizeof(SolarWindLisp::SimpleMatterFactory);
    print_sizeof(SolarWindLisp::IParser);
    print_sizeof(SolarWindLisp::SimpleParser);
    print_sizeof(SolarWindLisp::InterpreterIF);
    print_sizeof(SolarWindLisp::SimpleInterpreter);
    printf("\n");
    print_release_info();
    return 0;
}
