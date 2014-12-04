/*
 * file name:           src/repl_main.cc
 *
 * author:              Brian Yi ZHANG
 * email:               brianlions@gmail.com
 * date created:        Mon Nov 24 00:54:24 2014 CST
 */

#include <stdlib.h>
#include "solarwindlisp.h"

int main(int argc, char ** argv)
{
    SolarWindLisp::InterpreterIF::repl(argc > 1 ? argv[1] : NULL);
    exit(EXIT_SUCCESS);
}
