#ifndef J65_FILE_H
#define J65_FILE_H

#include <stdint.h>
#include <stdio.h>

#include "json65.h"

enum {
    J65_INSUFFICIENT_MEMORY    = -1,
    J65_IO_ERROR               = -2,
    J65_UNEXPECTED_END_OF_FILE = -3,
};

typedef void __fastcall__ (*j65_err_func) (FILE *err,
                                           void *ctx,
                                           int8_t status);

int8_t __fastcall__ j65_parse_file (FILE *f,
                                    void *scratch,
                                    size_t scratch_len,
                                    void *ctx,
                                    j65_callback cb,
                                    uint8_t max_depth,
                                    FILE *err,
                                    uint8_t width,
                                    const char *filename,
                                    j65_err_func user_err_func);

#endif  /* J65_FILE_H */
