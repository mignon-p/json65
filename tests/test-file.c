/* cl65 -I. -t apple2 -C apple2-system.cfg -o test.system -m test-file.map json65.s json65-file.c test-file.c */

#include <conio.h>

#include "json65-file.h"

static uint8_t scratch[1024];
static char filename[80];

static void errfunc (FILE *err, void *ctx, int8_t status) {
    fprintf (err, "Unknown error %d\n", status);
}

static int8_t callback (j65_parser *p, uint8_t event) {
    return 0;
}

int main (int argc, char **argv) {
    FILE *f;
    int8_t status;
    uint8_t width, height, i;

    screensize (&width, &height);

    for (i = 0 ; i < 100 ; i++) {
        snprintf (filename, sizeof (filename), "file%02d.json", i);

        printf ("\nParsing %s\n\n", filename);

        f = fopen (filename, "r");
        if (! f)
            break;

        status = j65_parse_file (f, scratch, sizeof (scratch),
                                 NULL, callback, 16,
                                 stderr, width, filename, errfunc);

        fclose (f);

        cgetc();
    }

    return 0;
}
