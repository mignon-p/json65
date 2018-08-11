/* cl65 -I. -t sim6502 -o test -m test.map json65.s test.c */

#include <stdio.h>
#include <string.h>
#include <json65.h>

static j65_state state;

static void callback (void *ctx, uint8_t event, int32_t data) {
    printf ("    Got event %u ($%02X) with data %ld\n", event, event, data);
}

static void test (char *str) {
    uint8_t ret;

    printf ("Testing '%s'\n", str);
    j65_init(&state);
    ret = j65_parse(NULL, callback, &state, str, strlen(str));
    printf ("Return value %u\n\n", ret);
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

    return 0;
}
