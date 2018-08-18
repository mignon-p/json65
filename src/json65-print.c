#include <stdbool.h>
#include "json65-print.h"
#include "json65-quote.h"

enum {
    ASCENDING,
    DESCENDING,
};

/* This is a non-recursive implementation because the tree might be deep
 * and the 6502 stack is not very deep. */
int __fastcall__ j65_print_tree (j65_node *root, FILE *f) {
    j65_node *n = root;
    uint8_t direction = DESCENDING;
    bool done = false;
    char c1, c2;
    j65_node *next;
    uint8_t next_direction;
    bool print_comma;
    uint8_t node_type;

    if (root == NULL)
        return 0;

    do {
        node_type = n->node_type;

        /* the default (for scalars) is to visit the next sibling */
        if (n->next) {
            next = n->next;
            next_direction = DESCENDING;
            print_comma = true;
        } else {
            next = n->parent;
            next_direction = ASCENDING;
            print_comma = false;
        }

        switch (node_type) {
        case J65_NULL:
            fputs ("null", f);
            break;
        case J65_FALSE:
            fputs ("false", f);
            break;
        case J65_TRUE:
            fputs ("true", f);
            break;
        case J65_INTEGER:
            fprintf (f, "%ld", n->u.integer);
            break;
        case J65_NUMBER:
            fputs (n->u.string, f);
            break;
        case J65_STRING:
            fputc ('\"', f);
            j65_print_escaped (n->u.string, f);
            fputc ('\"', f);
            break;
        case J65_KEY:
            if (direction == DESCENDING) {
                fputc ('\"', f);
                j65_print_escaped (n->u.pair.key, f);
                fputs ("\":", f);
                next = n->u.pair.value;
                next_direction = DESCENDING;
                print_comma = false;
            }
            break;
        case J65_START_OBJ:
        case J65_START_ARRAY:
            if (node_type == J65_START_OBJ) {
                c1 = '{';
                c2 = '}';
            } else {
                c1 = '[';
                c2 = ']';
            }
            if (direction == DESCENDING) {
                fputc (c1, f);
                next = n->u.child;
                if (next) {
                    next_direction = DESCENDING;
                } else {
                    next = n;
                    next_direction = ASCENDING;
                }
                print_comma = false;
            } else {
                fputc (c2, f);
            }
            break;
        default:
            /* should never happen... */
            fprintf (f, "?%u(%p)", node_type, n);
            break;
        }

        if (n == root && (direction == ASCENDING ||
                          (node_type != J65_START_OBJ &&
                           node_type != J65_START_ARRAY))) {
            done = true;
        } else if (print_comma) {
            fputc (',', f);
        }

        n = next;
        direction = next_direction;
    } while (!done && !ferror(f));

    if (ferror (f))
        return -1;
    else
        return 0;
}
