#ifndef J65_STRING_H
#define J65_STRING_H

#include <stdint.h>

/*
  j65_strings is a "string intern pool" used for "interning"
  string values:

  https://en.wikipedia.org/wiki/String_interning

  This saves space by only keeping a single copy of each string,
  and it also means interned strings can be compared by pointer
  value, without having to compare character-by-character.

  j65_strings is too large to fit on the stack, so it should be
  allocated statically or on the heap.

  To initialize the intern pool, call j65_init_strings().
  Since j65_intern_string() will allocate memory with malloc()
  for each unique interned string, you must call j65_free_strings()
  to free that memory when you are done with the intern pool.

  Internally, j65_strings is implemented as a fixed-size,
  256-entry hash table, where each hash bucket is a linked list
  to handle collisions.

  j65_strings has an implementation limit: It only supports
  strings of 255 bytes or less.  If a string is longer than
  255 bytes, the interned string is truncated at 255 bytes.
  This should not be an issue when interning strings returned
  by the JSON65 parser, since the JSON65 parser also has a
  limit of 255 bytes for strings.
 */
typedef struct {
    uint8_t opaque[512];
} j65_strings;

/*
  Initialize a string intern pool for use.  Once you have initialized
  it, you may call j65_intern_string() on it.
 */
void __fastcall__ j65_init_strings (j65_strings *strs);

/*
  Intern a string in the specified pool.  The returned pointer
  will strcmp() equal to the str argument, as long as the str
  argument is 255 bytes or less in length.  (If str is longer,
  then the interned string will be truncated to 255 bytes.)

  The returned pointer will be valid until j65_free_strings()
  is called on the pool.

  If the specified string does not already exist in the pool,
  and malloc() returns NULL when allocating memory for the new
  string, then j65_intern_string() will return NULL.
*/
const char * __fastcall__ j65_intern_string (j65_strings *strs,
                                             const char *str);

/*
  Frees all memory used by the given string pool.  Once
  j65_free_strings() is called, all of the pointers
  returned by j65_intern_string() for this pool become
  invalid.
 */
void __fastcall__ j65_free_strings (j65_strings *strs);

#endif  /* J65_STRING_H */
