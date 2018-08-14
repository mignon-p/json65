/* cl65 -I. -t sim6502 -o test -m test.map json65.s test.c */

#include <stdio.h>
#include <string.h>
#include <json65.h>

static j65_parser state;

static int8_t callback (j65_parser *p, uint8_t event) {
    printf ("    Got event %u", event);
    if (event == J65_INTEGER) {
        printf (" with integer %ld", j65_get_integer(p));
    }
    if (event == J65_INTEGER || event == J65_NUMBER ||
        event == J65_STRING  || event == J65_KEY) {
        printf (" with string '%s' of length %u",
                j65_get_string(p), j65_get_length(p));
    }
    printf (" at %lu:%lu\n",
            j65_get_line_number(p) + 1,
            j65_get_column_number(p) + 1);
    return 0;
}

static void test (char *str) {
    int8_t ret;

    printf ("Testing '%s'\n", str);
    j65_init(&state, NULL, callback, 255);
    ret = j65_parse(&state, str, strlen(str));
    printf ("Return value %d\n\n", ret);
}

int main (int argc, char **argv) {
    test ("[] ");
    test ("{} ");
    test ("1234 ");
    test ("-10000000 ");
    test ("1.5 ");
    test ("1e-2 ");
    test ("\"Hello, World\" ");
    test ("null ");
    test ("false ");
    test ("true ");
    test ("{\"foo\": 5} ");
    test ("{\"foo\": \"bar\", \"baz\": [1, 2, 3]} ");

    return 0;
}
