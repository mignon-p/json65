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
#include <stdlib.h>             /* malloc and free */
#include "json65-tree.h"

typedef struct {
    j65_strings strings;
    j65_node *root;
    j65_node *current;
    bool add_child;
} j65_tree_internal;

void __fastcall__ j65_init_tree (j65_tree *t) {
    j65_tree_internal *tree = (j65_tree_internal *) t;
    j65_init_strings (&tree->strings);
    tree->root = NULL;
    tree->current = NULL;
    tree->add_child = true;
}

int8_t __fastcall__ j65_tree_callback (j65_parser *p, uint8_t event) {
    j65_tree_internal *tree = (j65_tree_internal *) j65_get_context (p);
    const char *str = NULL;
    j65_node *n;

    switch (event) {
    case J65_END_OBJ:
    case J65_END_ARRAY:
        if (tree->add_child) {
            tree->add_child = false;
        } else {
            tree->current = tree->current->parent;
        }
        if (tree->current->parent &&
            tree->current->parent->node_type == J65_KEY) {
            tree->current = tree->current->parent;
        }
        return 0;
    case J65_NUMBER:
    case J65_STRING:
    case J65_KEY:
        str = j65_intern_string (&tree->strings, j65_get_string (p));
        if (str == NULL)
            return J65_OUT_OF_MEMORY;
    }

    n = (j65_node *) malloc (sizeof (j65_node));
    if (n == NULL)
        return J65_OUT_OF_MEMORY;

    n->node_type = event;
    n->location.line_offset = j65_get_line_offset (p);
    n->location.line_number = j65_get_line_number (p);
    n->location.column_number = j65_get_column_number (p);

    if (tree->add_child)
        n->parent = tree->current;
    else
        n->parent = tree->current->parent;
    n->next = NULL;

    n->integer = 0;           /* clears all fields of union to NULL */

    switch (event) {
    case J65_INTEGER:
        n->integer = j65_get_integer (p);
        break;
    case J65_KEY:
    case J65_NUMBER:
    case J65_STRING:
        n->string = str;
        break;
    }

    if (tree->current == NULL) {
        tree->root = n;
        tree->current = n;
    } else if (tree->add_child) {
        tree->current->child = n;
        if (tree->current->node_type == J65_KEY) {
            if (event == J65_START_OBJ || event == J65_START_ARRAY)
                tree->current = n;
        } else {
            tree->current = n;
        }
    } else {
        tree->current->next = n;
        tree->current = n;
    }

    switch (event) {
    case J65_KEY:
    case J65_START_OBJ:
    case J65_START_ARRAY:
        tree->add_child = true;
        break;
    default:
        tree->add_child = false;
        break;
    }

    return 0;
}

j65_node * __fastcall__ j65_find_key (j65_tree *t,
                                      j65_node *object,
                                      const char *key) {
    const char *k = j65_intern_string (&t->strings, key);

    if (k == NULL)
        return NULL;

    return j65_find_interned_key (object, k);
}

j65_node * __fastcall__ j65_find_interned_key (j65_node *object,
                                               const char *key) {
    j65_node *n;
    if (object->node_type == J65_START_OBJ)
        n = object->child;
    else if (object->node_type == J65_KEY)
        n = object;
    else
        return NULL;

    while (n != NULL) {
        if (n->string == key)
            return n;
        n = n->next;
    }

    return NULL;
}

void __fastcall__ j65_free_tree (j65_tree *t) {
    j65_tree_internal *tree = (j65_tree_internal *) t;
    j65_node *n = tree->root;
    j65_node *follow;

    /* traverse the entire tree without recursion */
    /* (since 6502 stack is limited) */
    while (n != NULL) {
        switch (n->node_type) {
        case J65_KEY:
        case J65_START_OBJ:
        case J65_START_ARRAY:
            follow = n->child;
            n->child = NULL;
            if (follow != NULL)
                break;
            /* fall thru */
        default:
            /* node has no children (either a leaf or empty container) */
            follow = n->next;
            if (follow == NULL) /* no remaining siblings, either */
                follow = n->parent;
            free (n);
        }
        n = follow;
    }

    tree->root = NULL;
    tree->current = NULL;
    tree->add_child = true;

    j65_free_strings (&tree->strings);
}
