/*
  JSON65 - A JSON parser for the 6502 microprocessor.

  https://github.com/ppelleti/json65

  Copyright © 2018 Patrick Pelletier

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

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
