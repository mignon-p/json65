#include <errno.h>
#include <string.h>
#include "json65-file.h"

static const char insufficient_memory[] = "insufficient memory";

static const char *get_errmsg (int8_t n) {
    switch (n) {
    case J65_PARSE_ERROR:        return "parse error";
    case J65_ILLEGAL_CHAR:       return "illegal character";
    case J65_ILLEGAL_ESCAPE:     return "illegal escape sequence";
    case J65_STRING_TOO_LONG:    return "string longer than 255 bytes";
    case J65_EXPECTED_STRING:    return "keys must be strings";
    case J65_EXPECTED_COLON:     return "expected ':'";
    case J65_EXPECTED_COMMA:     return "expected ','";
    case J65_EXPECTED_OBJ_END:   return "expected '}'";
    case J65_EXPECTED_ARRAY_END: return "expected ']'";
    default: return NULL;
    }
}

int8_t __fastcall__ j65_parse_file (FILE *f,
                                    void *scratch,
                                    size_t scratch_len,
                                    void *ctx,
                                    j65_callback cb,
                                    uint8_t max_depth,
                                    FILE *err,
                                    uint8_t width,
                                    const char *filename,
                                    j65_err_func user_err_func) {
    j65_parser *p;
    char *buf;
    size_t buflen;
    long origin;
    int8_t ret;
    size_t size;
    uint32_t line_num, column_num;
    const char *errmsg;
    uint32_t offset;

    if (scratch_len < sizeof (j65_parser) + width + 1) {
        fprintf (err, "%s %u\n", insufficient_memory, scratch_len);
        return J65_INSUFFICIENT_MEMORY;
    }

    p = (j65_parser *) scratch;
    buf = ((char *) scratch) + sizeof (j65_parser);
    buflen = scratch_len - sizeof (j65_parser);

    j65_init (p, ctx, cb, max_depth);
    /* I am having a weird problem with ftell returning 512 when
    ** I am at the beginning of the file.  Until I can figure that
    ** out, just assume that f is at the beginning of the file. */
#if 1
    origin = 0;
#else
    origin = ftell (f);
    if (origin < 0)
        goto io_error;
#endif

    do {
        size = fread (buf, 1, buflen, f);
        if (ferror (f))
            goto io_error;
        ret = j65_parse (p, buf, size);
    } while (ret == J65_WANT_MORE && !feof (f));

    if (ret == J65_WANT_MORE) {
        fprintf (err, "%s: Unexpected end of file\n", filename);
        return J65_UNEXPECTED_END_OF_FILE;
    }

    if (ret == J65_DONE) {
        return ret;
    }

    line_num = j65_get_line_number (p);
    column_num = j65_get_column_number (p);

    fprintf (err, "%s:%u:%u: ", filename, line_num + 1, column_num + 1);
    errmsg = get_errmsg (ret);
    if (errmsg != NULL) {
        fputs (errmsg, err);
    } else if (ret == J65_NESTING_TOO_DEEP) {
        fprintf (err, "exceeded max nesting depth of %u levels",
                 j65_get_max_depth (p));
    } else {
        if (user_err_func == NULL)
            user_err_func = j65_default_err_func;
        user_err_func (err, ctx, ret);
    }
    fputc ('\n', err);

    width--;
    offset = j65_get_line_offset (p);
    fprintf (err, "offset = %lu\n", offset);
    if (column_num >= width) {
        offset += (column_num - (width - 1));
        column_num = width - 1;
    }

    fprintf (err, "offset = %lu, origin = %lu\n", offset, origin);
    if (fseek (f, origin + offset, SEEK_SET) < 0)
        goto io_error;

    size = fread (buf, 1, width, f);
    fprintf (err, "fread = %u, feof = %u, ferror = %u\n", size, feof (f), ferror (f));
    buf[size] = 0;
    size = strcspn (buf, "\r\n");
    fprintf (err, "strcspn = %u\n", size);
    buf[size] = 0;
    fprintf (err, "%s\n", buf);

    memset (buf, ' ', column_num);
    buf[column_num] = '^';
    buf[column_num + 1] = 0;

    fprintf (err, "%s\n", buf);

    return ret;

 io_error:
    fprintf (err, "%s: %s\n", filename, _stroserror (_oserror));
    return J65_IO_ERROR;
}

void __fastcall__ j65_default_err_func (FILE *err,
                                        void *ctx,
                                        int8_t status) {
    if (status == J65_INSUFFICIENT_MEMORY) {
        fputs (insufficient_memory, err);
    } else {
        fprintf (err, "Unknown error code %d", status);
    }
}
