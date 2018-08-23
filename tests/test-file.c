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

#include <conio.h>

#include "json65-file.h"

static uint8_t scratch[1024];
static char filename[80];

static void errfunc (FILE *err, void *ctx, int8_t status) {
    fprintf (err, "Unknown error %d\n", status);
}

static int8_t callback (j65_parser *p, uint8_t event) {
    return 0;
}

int main (int argc, char **argv) {
    FILE *f;
    int8_t status;
    uint8_t width, height, i;

    screensize (&width, &height);

    for (i = 0 ; i < 100 ; i++) {
        snprintf (filename, sizeof (filename), "file%02d.json", i);

        printf ("\nParsing %s\n\n", filename);

        f = fopen (filename, "r");
        if (! f)
            break;

        status = j65_parse_file (f, scratch, sizeof (scratch),
                                 NULL, callback, 16,
                                 stderr, width, filename, errfunc);

        fclose (f);

        cgetc();
    }

    return 0;
}
