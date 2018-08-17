#include "json65-print.h"
#include "json65-quote.h"

int __fastcall__ j65_print_tree (j65_node *n, FILE *f) {
    uint8_t node_type = n->node_type;
    int ret;
    j65_node *n2;
    j65_node *n3;
    char c1, c2;

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
        fputc ('\"', f);
        j65_print_escaped (n->u.pair.key, f);
        fputs ("\":", f);
        ret = j65_print_tree (n->u.pair.value, f);
        if (ret < 0)
            return ret;
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
        fputc (c1, f);
        n2 = n->u.child;
        while (n2 != NULL) {
            ret = j65_print_tree (n2, f);
            if (ret < 0)
                return ret;
            n3 = n2->next;
            if (n3 != NULL)
                fputc (',', f);
            n2 = n3;
        }
        fputc (c2, f);
        break;
    default:
        /* should never happen... */
        fprintf (f, "?%u(%p)", node_type, n);
        break;
    }

    if (ferror (f))
        return -1;
    else
        return 0;
}
