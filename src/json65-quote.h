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

#ifndef J65_QUOTE_H
#define J65_QUOTE_H

#include <stdio.h>

/*
  Prints the specified string to the specified file handle.

  Special characters are escaped using the escape sequences
  from the JSON specification.

  Does not return a value, so to check for an error, you
  should call ferror(f).
 */
void __fastcall__ j65_print_escaped (const char *str, FILE *f);

#endif  /* J65_QUOTE_H */
