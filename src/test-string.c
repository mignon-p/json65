/* cl65 -I. -t sim6502 -o test-string -m test-string.map json65.s json65-string.s json65-tree.c test-string.c */

#include <stdio.h>
#include <string.h>
#include "json65-string.h"

static j65_strings strs;
static const char *results[300];
static char buf1[10];
static char buf2[10];

int main (int argc, char **argv) {
    uint16_t i;
    const char *tmp;

    j65_init_strings (&strs);

    for (i = 0 ; i < 300 ; i++) {
        snprintf (buf1, sizeof (buf1), "%u", i);
        results[i] = j65_intern_string (&strs, buf1);
        if (strcmp (buf1, results[i]) != 0) {
            printf ("first loop: '%s' (%p) not equal to '%s' (%p)\n",
                    results[i], results[i],
                    buf1, buf1);
            return 1;
        }
    }

    for (i = 0 ; i < 300 ; i++) {
        snprintf (buf2, sizeof (buf2), "%u", i);
        tmp = j65_intern_string (&strs, buf2);
        if (strcmp (buf2, tmp) != 0) {
            printf ("second loop: '%s' not equal to '%s'\n", tmp, buf2);
            return 1;
        }
        if (tmp != results[i]) {
            printf ("For '%s', %p not equal to %p\n", buf2, tmp, results[i]);
            return 1;
        }
    }

    printf ("Success!\n");
    return 0;
}
