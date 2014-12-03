#include <stdlib.h>
#include "solarwindlisp.h"

int main(int argc, char ** argv)
{
    SolarWindLisp::InterpreterIF::repl(argc > 1 ? argv[1] : NULL);
    exit(EXIT_SUCCESS);
}
