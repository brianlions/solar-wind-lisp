#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char ** argv)
{
    for (int i = 1; i < argc; ++i) {
        printf("[%d]: `%s' (%p)\n", i, argv[i], argv[i]);
        char * buf = strdup(argv[i]);
        printf("[%d]: buf `%s' (%p)\n", i, buf, buf);
        free(buf);
    }
    return 0;
}
