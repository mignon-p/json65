#ifndef J65_TREE_H
#define J65_TREE_H

#include "json65.h"
#include "json65-string.h"

/* in addition to the status codes from j65_status */
enum {
    J65_OUT_OF_MEMORY = -1,
};

typedef struct {
    uint32_t line_offset;
    uint32_t line_number;
    uint32_t column_number;
} j65_source_location;

typedef struct j65_node j65_node;

typedef struct {
    const char *key;
    j65_node *value;
} j65_pair;

struct j65_node {
    uint8_t node_type;
    j65_source_location location;
    j65_node *parent;
    j65_node *next;
    union {
        int32_t integer;        /* J65_INTEGER */
        const char *string;     /* J65_NUMBER or J65_STRING */
        j65_node *child;        /* J65_START_OBJ or J65_START_ARRAY */
        j65_pair pair;          /* J65_KEY */
    } u;
};

typedef struct {
    j65_strings strings;
    j65_node *root;
    uint8_t internal[3];
} j65_tree;

void __fastcall__ j65_init_tree (j65_tree *t);

int8_t __fastcall__ j65_tree_callback (j65_parser *p, uint8_t event);

j65_node * __fastcall__ j65_find_key (j65_tree *t,
                                      j65_node *object,
                                      const char *key);

j65_node * __fastcall__ j65_find_interned_key (j65_node *object,
                                               const char *key);

void __fastcall__ j65_free_tree (j65_tree *t);

#endif  /* J65_TREE_H */
