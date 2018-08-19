/* cl65 -I. -t sim6502 -o test-string -m test-string.map json65-string.s test-string.c */

#include <stdio.h>
#include <string.h>
#include "json65-string.h"

#define ITERATIONS 300

static j65_strings strs;
static const char *results[ITERATIONS];
static char buf1[10];
static char buf2[10];

static void print_bucket_usage (void) {
    uint8_t *lo;
    uint8_t *hi;
    uint16_t i, used = 0;

    lo = (uint8_t *) &strs;
    hi = lo + 256;

    for (i = 0 ; i < 256 ; i++) {
        if (lo[i] || hi[i])
            used++;
    }

    printf ("used %u/256 buckets\n", used);
}

int main (int argc, char **argv) {
    uint16_t i;
    const char *tmp;

    j65_init_strings (&strs);

    for (i = 0 ; i < ITERATIONS ; i++) {
        snprintf (buf1, sizeof (buf1), "%u", i);
        results[i] = j65_intern_string (&strs, buf1);
        if (strcmp (buf1, results[i]) != 0) {
            printf ("first loop: '%.20s' (%p) not equal to '%s' (%p)\n",
                    results[i], results[i],
                    buf1, buf1);
            return 1;
        }
    }

    for (i = 0 ; i < ITERATIONS ; i++) {
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

    print_bucket_usage ();
    j65_free_strings (&strs);

    printf ("Success!\n");
    return 0;
}
