/*
 * file name:           poc/strtol.cc
 *
 * author:              Brian Yi ZHANG
 * email:               brianlions@gmail.com
 * date created:        Fri Nov 21 01:08:59 2014 CST
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
int main(int argc, char ** argv)
{
    for (int i = 1; i < argc; ++i) {
        char * buf = argv[i];
        long int v = strtol(buf, NULL, 0);
        printf("`%s'\t%ld (0x%08lx)\n", buf, v, static_cast<uint64_t>(v));
    }
    return 0;
}
