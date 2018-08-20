#ifndef J65_QUOTE_H
#define J65_QUOTE_H

#include <stdio.h>

/*
  Prints the specified string to the specified file handle.

  Special characters are escaped using the escape sequences
  from the JSON specification.

  Does not return a value, so to check for an error, you
  should call ferror(f).
 */
void __fastcall__ j65_print_escaped (const char *str, FILE *f);

#endif  /* J65_QUOTE_H */
