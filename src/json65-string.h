#ifndef J65_STRING_H
#define J65_STRING_H

#include <stdint.h>

typedef struct {
    uint8_t opaque[512];
} j65_strings;

void __fastcall__ j65_init_strings (j65_strings *strs);

const char * __fastcall__ j65_intern_string (j65_strings *strs,
                                             const char *str);

void __fastcall__ j65_free_strings (j65_strings *strs);

#endif  /* J65_STRING_H */
