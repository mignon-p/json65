#include <stdio.h>
#include <string.h>
#include "json65-tree.h"

static char buf[1024];
static j65_parser parser;
static j65_tree tree;
static const char infile[] = "test-tree.json";

static int do_test (size_t len) {
    int8_t status;
    j65_node *n;
    uint8_t node_type;
    const char *str;
    uint32_t line_number, column_number;

    j65_init_tree (&tree);
    j65_init (&parser, &tree, j65_tree_callback, 255);
    status = j65_parse (&parser, buf, len);
    if (status != J65_DONE) {
        fprintf (stderr, "j65_parse returned status %d\n", status);
        return 1;
    }

    n = j65_find_key (&tree, tree.root, "color");
    if (n == NULL) {
        fprintf (stderr, "couldn't find color\n");
        return 1;
    }

    n = n->u.ptrs.child;
    n = j65_find_key (&tree, n, "linearCutoff");
    if (n == NULL) {
        fprintf (stderr, "couldn't find linearCutoff\n");
        return 1;
    }

    n = n->u.ptrs.child;
    node_type = n->node_type;
    if (node_type != J65_NUMBER) {
        fprintf (stderr, "%u != J65_NUMBER\n", node_type);
        return 1;
    }

    str = n->u.ptrs.string;
    if (0 != strcmp (str, "0.0078125")) {
        fprintf (stderr, "%s != 0.0078125\n", str);
        return 1;
    }

    line_number = n->location.line_number + 1;
    column_number = n->location.column_number;
    if (line_number != 8) {
        fprintf (stderr, "line number %lu != 8\n", line_number);
        return 1;
    }

    if (column_number != 33) {
        fprintf (stderr, "column number %lu != 33\n", column_number);
        return 1;
    }

    if (n->location.line_offset != 135) {
        fprintf (stderr, "line offset %lu != 135\n", n->location.line_offset);
        return 1;
    }

    n = j65_find_key (&tree, tree.root, "banana");
    if (n != NULL) {
        fprintf (stderr, "found banana but shouldn't have\n");
        return 1;
    }

    j65_free_tree (&tree);
    return 0;
}

int main (int argc, char **argv) {
    FILE *f;
    int badness = 0;
    size_t len;

    f = fopen (infile, "r");
    if (! f) {
        fprintf (stderr, "Couldn't open file '%s' for reading\n", infile);
        return 1;
    }

    len = fread (buf, 1, sizeof (buf), f);
    if (ferror (f)) {
        fprintf (stderr, "Couldn't read file '%s'\n", infile);
        return 1;
    }

    fclose (f);

    badness = do_test (len);

    if (badness == 0)
        fprintf (stderr, "Success!\n");

    return badness;
}
