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

#ifndef J65_FILE_H
#define J65_FILE_H

#include <stdint.h>
#include <stdio.h>

#include "json65.h"

/*
  These are additional error codes that can be returned, besides
  the ones in j65_status.
 */
enum {
    J65_INSUFFICIENT_MEMORY    = -1, /* scratch_len was too small */
    J65_IO_ERROR               = -2, /* file I/O returned an error */
    J65_UNEXPECTED_END_OF_FILE = -3, /* incomplete JSON value */
};

/*
  This optional callback function should translate your custom
  error code (in the status argument) to a human-readable
  string, and print it on the err filehandle.  The ctx argument
  is supplied in case you want your error message to contain
  additional information from the context.

  If you do not recognize the error code, you can pass it along
  to j65_default_err_func().
 */
typedef void __fastcall__ (*j65_err_func) (FILE *err,
                                           void *ctx,
                                           int8_t status);

/*
  Parses JSON from a file.  If an error occurs, prints a nice
  human-readable error message, including the line number and
  column number, and prints the offending line with a caret
  pointing to the offending position.

  Does not do any dynamic memory allocation.  However, it requires a
  chunk of scratch memory.  The scratch memory must be at least 513 +
  width bytes, and I would recommend at least 768 bytes, though 1K or
  2K might be preferable, from a performance standpoint.  (But,
  experiment and see what works best on your operating system.)
  The scratch buffer does not need to be big enough to hold the whole
  file, though.

  Returns J65_DONE if the file is parsed successfully.  If an error
  occurs, returns a negative number which will either be one of the
  error codes from j65_status in json65.h, or one of the error codes
  in the enumation at the top of this file.  In the case of an error,
  a human-readable message will have been printed to the err
  filehandle (so you may not need to care which negative value was
  returned, just that it was negative).  Never returns J65_WANT_MORE.
  (Feeding chunks of the file is handled automatically, and if
  J65_WANT_MORE occurs at the end of the file, that is translated
  into J65_UNEXPECTED_END_OF_FILE.)

  Here are the arguments:

  f - The file to parse.

  scratch - Pointer to scratch memory.

  scratch_len - Size of scratch memory, in bytes.  If it is
  not at least 513 + width bytes, the function returns
  J65_INSUFFICIENT_MEMORY.

  ctx - Context argument which will be passed to callback.  (To pass
  a pointer to any data structure you desire.)

  cb - The callback which is called for each event in the JSON file.
  For more information, see documentation in json65.h.

  max_depth - The maximum nesting depth (of nested arrays and objects)
  allowed.  If you do not care, set it to 0.  See documentation for
  j65_init() in json.h for additional information.

  err - The file handle (such as stdout, stderr, or an actual file)
  to print the error message to, if an error occurs.

  width - The width, in characters, of the display device for the
  err filehandle.  If the err filehandle represents the screen, you
  can get the width with the screensize() function from the cc65
  standard header file conio.h.  If err is a real file, then just
  choose something reasonable, like 80.

  filename - The name of the file represented by the f filehandle.
  This name is only used in generating an error message; we do not
  attempt to access the file by name.

  user_err_func - If you return custom error codes from your
  callback function, you can supply a user_err_func which
  translates your custom error codes to a human-readable string.
  Passing NULL is the same as passing j65_default_err_func.
 */
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

/*
  A default implementation of j65_err_func, which prints
  unknown error codes numerically.
 */
void __fastcall__ j65_default_err_func (FILE *err,
                                        void *ctx,
                                        int8_t status);

#endif  /* J65_FILE_H */
