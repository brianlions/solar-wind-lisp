#include <stdio.h>
#include <stdint.h>
#include "swl_component.h"
#include "swl_expr.h"
#include "release.h"

#define print_header()                          \
do {                                            \
    printf("  size  type name\n");              \
    printf("------  -----------\n");            \
} while (0)

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
    print_sizeof(SolarWindLisp::IExpr);
    print_sizeof(SolarWindLisp::IExpr::expr_type);
    print_sizeof(SolarWindLisp::Expr);
    print_sizeof(SolarWindLisp::Expr::AtomData);
    print_sizeof(SolarWindLisp::CompositeExpr);
    printf("\n");
    print_release_info();
    return 0;
}
