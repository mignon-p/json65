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

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <cc65.h>

#include "json65.h"
#include "json65-file.h"
#include "json65-tree.h"
#include "json65-print.h"

static uint8_t scratch[2048];
static j65_tree tree;

static void my_exit (int code) __attribute__ ((noreturn)) {
    if (doesclrscrafterexit ()) {
        cursor (true);
        cgetc ();
    }
    exit (code);
}

int main (int argc, char **argv) {
    const char *filename;
    FILE *f;
    int8_t ret;
    j65_node *banana, *banana_value;
    uint8_t width, height;

    if (argc >= 2) {
        filename = argv[1];
    } else {
        filename = "input.json";
    }

    /* Find out the width of the screen.  (This will be used
    ** when formatting error messages.)
    */
    screensize (&width, &height);

    /* Initialize the tree data structure. */
    j65_init_tree (&tree);

    /* Open the input file. */
    f = fopen (filename, "r");
    if (!f) {
        perror (filename);
        my_exit (EXIT_FAILURE);
    }

    /* Call j65_parse_file() to parse the file, and it will in turn
    ** call j65_tree_callback() to build up the tree.
    */
    ret = j65_parse_file (f,                 /* file to parse */
                          scratch,           /* pointer to a scratch buffer */
                          sizeof (scratch),  /* length of scratch buffer */
                          &tree,             /* "context" for callback */
                          j65_tree_callback, /* the callback function */
                          0,                 /* 0 means use max nesting depth */
                          stderr,            /* where to print errors */
                          width,             /* width of screen (for errors) */
                          filename,          /* used in error messages */
                          NULL);             /* no custom error func */
    if (ret < 0) {
        /* Don't need to print any error message here, because
        ** j65_parse_file() already printed an error message before
        ** returning a negative number.
        */
        fclose (f);
        my_exit (EXIT_FAILURE);
    }

    /* We're done reading the file, so we can close it now. */
    fclose (f);

    /* Look for a key named "banana". */
    banana = j65_find_key (&tree, tree.root, "banana");
    if (banana == NULL) {
        printf ("Could not find banana.\n");
        j65_free_tree (&tree);
        my_exit (EXIT_FAILURE);
    }

    /* The variable "banana" now points to the key "banana" in
    ** the tree.  If we want to know the value associated with
    ** the key "banana", we need to look at the key's child node.
    */
    banana_value = banana->child;

    /* Now print the value associated with the key "banana". */
    printf ("Here are some fun facts about bananas, from line %lu of %s:\n",
            banana_value->location.line_number + 1, filename);
    ret = j65_print_tree (banana_value, stdout);
    if (ret < 0) {
        perror ("error printing tree");
        j65_free_tree (&tree);
        my_exit (EXIT_FAILURE);
    }

    /* j65_print_tree() prints everything on one line (so, not
    ** particularly human readable) without a newline at the
    ** end, so we must print the newline ourselves.
    */
    printf ("\n");

    /* We are done, so we can free the tree now. */
    j65_free_tree (&tree);

    my_exit (EXIT_SUCCESS);
}
