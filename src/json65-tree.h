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

#ifndef J65_TREE_H
#define J65_TREE_H

#include "json65.h"
#include "json65-string.h"

/* in addition to the status codes from j65_status */
enum {
    J65_OUT_OF_MEMORY = -1,     /* malloc returned NULL */
};

/*
  Specifies the location in the input JSON file that corresponds
  to a particular node.

  line_number and column_number are 0-based, so you should probably
  add 1 before displaying them to the user.

  line_offset is the byte position in the file where the line
  specified by line_number begins.  This is useful if you want
  to display the line in question, because you can seek directly
  to the beginning of the line in the file.

  For arrays and objects, the position specified is that of the
  open square bracket or open curly brace.  For scalars, the
  position specified is at the end of the scalar.
 */
typedef struct {
    uint32_t line_offset;
    uint32_t line_number;
    uint32_t column_number;
} j65_source_location;

typedef struct j65_node j65_node;

/*
  j65_node represents a value parsed from the JSON file.

  The node_type is recycled from the j65_event enumeration
  in json65.h.  All event types are legal node types, with
  the exception of J65_END_OBJ and J65_END_ARRAY.  Since
  the tree structure treats objects and arrays as containers
  rather than events, they do not have a separate start and
  end.  Rather, J65_START_OBJ and J65_START_ARRAY are used
  to specify object nodes and array nodes, respectively.

  There are three types of container nodes: J65_START_ARRAY,
  J65_START_OBJ, and J65_KEY.  Array nodes may contain any
  number of children, of any type except J65_KEY.  Object
  nodes may contain any number of children, but they may
  only be of type J65_KEY.  Key nodes must have exactly one
  child, which may be of any type except J65_KEY.

  Every node except the root node has a parent, which is the
  J65_START_ARRAY, J65_START_OBJ, or J65_KEY node which
  contains it.  For the root node, the parent pointer is NULL.

  Container nodes point to their first child.  Each child
  node points to its next sibling via the next pointer.
  The final sibling has a NULL next pointer.
 */
struct j65_node {
    uint8_t node_type;
    j65_source_location location;
    j65_node *parent;
    j65_node *next;
    union {
        int32_t integer;        /* J65_INTEGER */
        struct {
            const char *string; /* J65_KEY, J65_NUMBER, or J65_STRING */
            j65_node *child;    /* J65_KEY, J65_START_OBJ, or J65_START_ARRAY */
        };
    };
};

/*
  This structure represents an entire tree of nodes read from
  a JSON file.  The j65_tree should be initialized with
  j65_init_tree(), and then it can be passed as the context
  argument to j65_parse(), with j65_tree_callback() as the
  callback argument.

  Once parsing has completed, the tree will be populated.
  The root pointer points to the root (top-level) node.
  All of the strings contained in the tree (in J65_STRING,
  J65_NUMBER, and J65_KEY nodes) will be interned in the
  strings member.

  Besides the string pool and the root pointer, j65_tree also
  contains a small amount of bookkeeping information used
  by the callback, which is unused once parsing is complete.
 */
typedef struct {
    j65_strings strings;
    j65_node *root;
    uint8_t internal[3];
} j65_tree;

/*
  Initializes the j65_tree structure for use.
 */
void __fastcall__ j65_init_tree (j65_tree *t);

/*
  This should be specified as the callback to j65_parse(),
  and the j65_tree structure should be specified as the
  context.  This callback builds up the tree as the parser
  generates events.
 */
int8_t __fastcall__ j65_tree_callback (j65_parser *p, uint8_t event);

/*
  Interns the given string into the tree's intern pool, and
  then searches for a J65_KEY child node of the given
  J65_START_OBJ node whose key matches the given string.
  The matching J65_KEY node is returned, or NULL if there is
  no match.
 */
j65_node * __fastcall__ j65_find_key (j65_tree *t,
                                      j65_node *object,
                                      const char *key);

/*
  Searches for a key node which matches the given string.  The
  string is expected to have already been interned in the
  tree's string intern pool.  The node passed in should be
  of type J65_START_OBJ, and its J65_KEY children are
  searched.  The matching J65_KEY node is returned, or NULL if
  there is no match.
 */
j65_node * __fastcall__ j65_find_interned_key (j65_node *object,
                                               const char *key);

/*
  Frees all the memory used by this tree.  The tree is traversed,
  and all of the nodes are freed.  Additionally, j65_free_strings()
  is called on the string intern pool contained within the
  j65_tree structure.
 */
void __fastcall__ j65_free_tree (j65_tree *t);

#endif  /* J65_TREE_H */
