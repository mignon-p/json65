/* cl65 -I. -t sim6502 -o test-print -m test-print.map json65.s json65-string.s json65-tree.c json65-quote.s json65-print.c test-print.c */

#include <string.h>
#include "json65-print.h"

static char buf1[2048];
static char buf2[2048];
static j65_parser parser;
static j65_tree tree;
static const char infile[] = "test-print.json";
static const char outfile[] = "json.test.print.tmp";

static int do_test (void) {
    int8_t status;
    size_t len;
    FILE *f;
    int ret;

    printf ("starting parse\n");

    j65_init_tree (&tree);
    j65_init (&parser, &tree, j65_tree_callback, 16);
    len = strlen (buf1);
    status = j65_parse (&parser, buf1, len);
    if (status != J65_DONE) {
        fprintf (stderr, "status %d\n", status);
        return 1;
    }

    printf ("finished parse\n");

    f = fopen (outfile, "w");
    if (! f) {
        fprintf (stderr, "Couldn't open file '%s' for writing\n", outfile);
        return 1;
    }

    ret = j65_print_tree (tree.root, f);
    fputc ('\n', f);
    fclose (f);

    printf ("printed tree\n");

    if (ret < 0) {
        fprintf (stderr, "Error writing file\n");
        return 1;
    }

    f = fopen (outfile, "r");
    if (! f) {
        fprintf (stderr, "Couldn't open file '%s' for reading\n", outfile);
        return 1;
    }

    if (! fgets (buf2, sizeof (buf2), f)) {
        fprintf (stderr, "Couldn't read file\n");
        return 1;
    }

    fclose (f);

    if (0 != strcmp (buf1, buf2)) {
        fprintf (stderr, "strings not equal:\n%s%s\n", buf1, buf2);
        return 1;
    }

    printf ("Done!\n");

    j65_free_tree (&tree);

    return 0;
}

int main (int argc, char **argv) {
    FILE *f;
    int badness = 0;

    f = fopen (infile, "r");
    if (! f) {
        fprintf (stderr, "Couldn't open file '%s' for reading\n", infile);
        return 1;
    }

    while (fgets (buf1, sizeof (buf1), f)) {
        badness += do_test ();
    }

    fclose (f);

    if (badness == 0)
        fprintf (stderr, "Success!\n");

    return badness;
}
