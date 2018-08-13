#ifndef J65_H
#define J65_H

#include <stdint.h>

enum j65_event {
    /* events */
    J65_NULL        = 0,
    J65_FALSE       = 1,
    J65_TRUE        = 2,
    J65_INTEGER     = 3,        /* integer */
    J65_NUMBER      = 4,        /* string */
    J65_STRING      = 5,        /* string */
    J65_KEY         = 6,        /* string */
    J65_START_OBJ   = 7,
    J65_END_OBJ     = 8,
    J65_START_ARRAY = 9,
    J65_END_ARRAY   = 10,
    /* errors */
    J65_ILLEGAL_CHAR       = 0x80, /* integer (byte offset of error) */
    J65_ILLEGAL_ESCAPE     = 0x81, /* integer (byte offset of error) */
    J65_NESTING_TOO_DEEP   = 0x82, /* integer (byte offset of error) */
    J65_STRING_TOO_LONG    = 0x83, /* integer (byte offset of error) */
    J65_PARSE_ERROR        = 0x84, /* integer (byte offset of error) */
    J65_EXPECTED_STRING    = 0x85, /* integer (byte offset of error) */
    J65_EXPECTED_COLON     = 0x86, /* integer (byte offset of error) */
    J65_EXPECTED_COMMA     = 0x87, /* integer (byte offset of error) */
    J65_EXPECTED_OBJ_END   = 0x88, /* integer (byte offset of error) */
    J65_EXPECTED_ARRAY_END = 0x89, /* integer (byte offset of error) */
};

enum j65_status {
    J65_DONE      = 0,
    J65_WANT_MORE = 1,
    J65_ERROR     = 2,
};

typedef struct {
    uint8_t dummy[512];
} j65_state;

typedef struct {
    size_t len;
    const char *ptr;
} j65_string;

typedef union {
    int32_t val;
    j65_string str;
} j65_union;

typedef void __fastcall__ (*j65_callback)(void *ctx, j65_state *s, uint8_t event, int32_t data);

uint8_t __fastcall__ j65_parse(void *ctx, j65_callback cb, j65_state *s, const char *buf, size_t len);

void __fastcall__ j65_init(j65_state *s);

const char * __fastcall__ j65_get_string(const j65_state *s);

size_t __fastcall__ j65_get_length(const j65_state *s);

long __fastcall__ j65_get_long(const j65_state *s);

#endif  /* J65_H */
