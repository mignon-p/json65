/* cl65 -I. -t sim6502 -o test -m test.map json65.s test.c */

#include <stdio.h>
#include <string.h>
#include <json65.h>

static j65_state state;

static void callback (void *ctx, uint8_t event, int32_t data) {
    printf ("Got event %u ($%02X)\n", event, event);
}

int main (int argc, char **argv) {
    uint8_t ret;
    const char *str = "[] ";

    j65_init(&state);
    ret = j65_parse(NULL, callback, &state, str, strlen(str));
    printf ("Return value %u\n", ret);
    return 0;
}
