#ifndef J65_PRINT_H
#define J65_PRINT_H

#include <stdio.h>
#include "json65-tree.h"

/*
  Prints the specified tree to the specified file handle as JSON.

  Returns 0 on success.  If an error occurs on the file handle,
  returns -1.  In that case, look at errno and/or _oserror to
  see what the error was.
 */
int __fastcall__ j65_print_tree (j65_node *n, FILE *f);

#endif  /* J65_PRINT_H */
