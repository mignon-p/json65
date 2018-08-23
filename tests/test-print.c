/*
  JSON65 - A JSON parser for the 6502 microprocessor.

  https://github.com/ppelleti/json65

  Copyright © 2018 Patrick Pelletier

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

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

    j65_init_tree (&tree);
    j65_init (&parser, &tree, j65_tree_callback, 255);
    len = strlen (buf1);
    status = j65_parse (&parser, buf1, len);
    if (status != J65_DONE) {
        fprintf (stderr, "status %d\n", status);
        return 1;
    }

    f = fopen (outfile, "w");
    if (! f) {
        fprintf (stderr, "Couldn't open file '%s' for writing\n", outfile);
        return 1;
    }

    ret = j65_print_tree (tree.root, f);
    fputc ('\n', f);
    fclose (f);

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
