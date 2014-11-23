#include <stdlib.h>
#include "solarwindlisp.h"

int main(int argc, char ** argv)
{
    SolarWindLisp::InterpreterIF::repl(true);
    exit(EXIT_SUCCESS);
}
