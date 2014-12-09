/*
 * file name:           src/sizeof_main.cc
 *
 * author:              Brian Yi ZHANG
 * email:               brianlions@gmail.com
 * date created:        Thu Nov 20 16:00:20 2014 CST
 */

#include <stdio.h>
#include <stdint.h>
#include "release.h"
#include "expr.h"
#include "parser.h"
#include "future.h"
#include "scoped_env.h"
#include "proc.h"
#include "prim_proc.h"
#include "interpreter.h"
#include "random.h"

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
    print_sizeof(SolarWindLisp::MatterIF);
    print_sizeof(SolarWindLisp::MatterIF::matter_type_t);
    print_sizeof(SolarWindLisp::Atom);
    print_sizeof(SolarWindLisp::Atom::atom_type_t);
    print_sizeof(SolarWindLisp::Atom::AtomData);
    print_sizeof(SolarWindLisp::CompositeExpr);
    print_sizeof(SolarWindLisp::PrimProcIF);
    print_sizeof(SolarWindLisp::Proc);
    print_sizeof(SolarWindLisp::Future);
    print_sizeof(SolarWindLisp::ScopedEnv);
    print_sizeof(SolarWindLisp::MatterFactoryIF);
    print_sizeof(SolarWindLisp::SimpleMatterFactory);
    print_sizeof(SolarWindLisp::ParserIF);
    print_sizeof(SolarWindLisp::SimpleParser);
    print_sizeof(SolarWindLisp::InterpreterIF);
    print_sizeof(SolarWindLisp::SimpleInterpreter);
    print_sizeof(SolarWindLisp::MatterPtr);
    print_sizeof(SolarWindLisp::Utils::Prng);
    printf("\n");
    print_release_info();
    return 0;
}
