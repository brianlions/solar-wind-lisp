/*
 * file name:           src/pbhrepl_main.cc
 *
 * author:              Brian Yi ZHANG
 * email:               brianlions@gmail.com
 * date created:        Mon Jun 15 16:58:35 2015 CST
 */

#include <stdlib.h>
#include <iostream>

#include "solarwindlisp.h"
#include "protobuf_heavy_interp.h"
#include "foo.pb.h"

extern int foo_build_and_serialize(void * buf, int size, Bar ** ptr_msg = NULL);

int main(int argc, char ** argv)
{
    char buffer[1024 * 16];
    int len = 0;
    if ((len = foo_build_and_serialize(buffer, sizeof(buffer))) < 0) {
        exit(EXIT_FAILURE);
    }

    SolarWindLisp::ProtobufHeavyInterpreter pbh_inter;
    if (pbh_inter.pre_initialization(buffer, len, "./proto/foo.proto", "Bar")
            != 0) {
        exit(EXIT_FAILURE);
    }

    if (!pbh_inter.initialize()) {
        exit(EXIT_FAILURE);
    }
    pbh_inter.interactive(argc > 1 ? argv[1] : NULL);
    exit(EXIT_SUCCESS);
}
