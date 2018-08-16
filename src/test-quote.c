/* cl65 -I. -t sim6502 -o test-quote -m test-quote.map json65-quote.s test-quote.c */

#include "json65-quote.h"

static void do_test (const char *s) {
    fputc ('\"', stdout);
    j65_print_escaped (s, stdout);
    fputs ("\"\n", stdout);
}

int main (int argc, char **argv) {
    do_test ("Hello, World!");
    do_test ("Hello, World!\n");
    do_test ("Hello,\r\nWorld!");
    do_test ("Hello, \"World!\"");
    do_test ("\aHello, World!");
    do_test ("Backslash \\");
    do_test ("Hello,\tWorld!");
    do_test ("\001\002\003");

    return 0;
}
