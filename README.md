I was watching TV, and there was a commercial which proclaimed, "It's
time to do what *you* want!"  I replied to the TV, "It's time to write
a JSON parser in 6502 assembly language?"  Somehow I don't think
that's what they had in mind, but the TV is right, I *should* do what
I want.

So, here is my JSON parser.  The core parser is written entirely in
6502 assembly language, and is meant to be assembled with [ca65][1].
However, it is meant to be called from C, and uses the
[cc65 calling convention][2] (specifically, the `fastcall` convention).

JSON65 should work on any processor in the 6502 family.  (It does not
use any 65C02 instructions.)

The assembly language parts of JSON65 use the zero page locations used
by `cc65`, in a way which is compatible with the C calling convention.

JSON65 should work on any target supported by the `cc65` toolchain.  I
have tested it on `sim65` and on an unenhanced Apple //e.

## Parser (json65.h)

JSON65 is an event-driven (SAX-style) parser, so the parser is given a
callback function, which it calls for each event.

JSON65 supports incremental parsing, so you can freely feed it any
sized chunks of input, and you don't need to have the whole file in
memory at once.

JSON65 is fully reentrant, so you can incrementally parse several
files at once if you so desire.

JSON65 does have a couple of limits: strings are limited to 255 bytes,
and the nesting depth (of nested arrays or objects) is limited to 224.
However, there is no limit on the length of a line, or the length of a
file.

JSON65 uses 512 bytes of memory for each parser, which must be
allocated by the caller.  JSON65 does not use dynamic memory
allocation.

In accordance with the [JSON specification][3], JSON65 assumes its
input is UTF-8 encoded.  However JSON65 does not validate the UTF-8,
so any encoding can be used, as long as all bytes with the high bit
clear represent ASCII characters.  Bytes with the high bit set are
only allowed inside strings.  The only place where JSON65 assumes
UTF-8 is in the processing of `\u` escape sequences.  In accordance
with the JSON specification, a single `\u` escape can be used to
specify code points in the Basic Multilingual Plane, and two
consecutive `\u` escapes (a UTF-16 surrogate pair) can be used to
specify a code point outside the Basic Multilingual Plane.  These
escapes will be translated into the proper UTF-8.

Because JSON only allows newlines in places where arbitrary whitespace
is allowed, JSON65 is agnostic to the type of line ending.  (CR, LF,
or CRLF.)  For the purposes of counting line numbers for error
reporting, JSON65 handles CR, LF, or CRLF line endings.

JSON65 will parse numbers which fit into a 32-bit signed long, and
will provide the long to the callback.  All other numbers
(i. e. floating point numbers, or integers which overflow a 32-bit
long) are provided to the callback as a string.  (Like strings,
numbers cannot be more than 255 digits long.)

The callback function may return an error if it wishes.  This will
cause parsing to stop immediately, and the error code returned by the
callback will be returned by `j65_parse()`.  Error codes are negative
numbers, and the user may use the codes from `J65_USER_ERROR` to
`-1`, inclusive, for their own error codes.

## Tree interface (json65-tree.h)

If you use the event-driver parser, you'll need to build your own data
structure (or otherwise handle the data somehow) as the events come
in.  If you don't want to do that, you can use the tree interface
(`json65-tree.h`) instead, which builds up a data structure for you.
This only works for small files, because the entire tree has to fit in
memory at once.

Unlike the event-based parser, the tree interface uses dynamic memory
allocation.

## Printing JSON (json65-print.h)

Mostly, JSON65 is a parser.  However, it does have some support for
printing JSON back to a file, in `json65-print.h`.  The function
`j65_print_tree()` will print a JSON tree (from the tree interface in
`json65-tree.h`) to a given filehandle.  It prints the entire JSON
tree on a single line with no whitespace.  This is the most compact
format for a machine-readable JSON file, but it is not particularly
human-readable.

If you write your own code to print JSON, either because you want to
pretty-print it, or because you are using a data structure other than
`j65_node`, you may still want to use the function
`j65_print_escaped()` from `json65-quote.h`.  It handles escaping a
string using the JSON escape sequences.

## API documentation

I don't have any fancy Doxygen documentation, but the API is
documented by comments in the header files.  If you wish to use the
event-driven parser, read [json65.h](src/json65.h).  If you wish to
use the tree interface, read [json65-tree.h](src/json65-tree.h).

## Library organization

If you simply wish to use the event-driven (SAX-style) parser, you
only need one header file (`json65.h`) and one assembly file
(`json65.s`).  However, there are some helper functions in other
files, which you can optionally use with JSON65 if you like.  Most
notable is the tree interface to JSON65, which you may use instead of
the event-driven interface for small files.

Each header file corresponds directly to one implementation file.
Some of the implementation files are written in assembly language, and
some are written in C.  Here is a description of each, along with the
size of the machine code of the implementation (`CODE` section plus
`RODATA` section; none of the implementation files have any `DATA` or
`BSS`).

* [json65.h](src/json65.h) (2240 bytes) - The core, event-driven
  parser.  This is the only file that is required if you wish to build
  your own data structure.
* [json65-string.h](src/json65-string.h) (291 bytes) - This implements
  a [string intern pool][4] which is used by the tree interface.
* [json65-tree.h](src/json65-tree.h) (1300 bytes) - The tree
  interface, which builds up a tree data structure as the file is
  parsed.  You may then traverse the tree to your heart's content.
* [json65-quote.h](src/json65-quote.h) (226 bytes) - This has a
  function which prints strings, replacing special characters with the
  escape sequences from the JSON specification.  It is used by the
  tree printer, but can also be used standalone if you are printing
  JSON files yourself without using the tree interface.
* [json65-print.h](src/json65-print.h) (710 bytes) - Prints a tree to
  a file as JSON.  Use this if you are using the tree interface, and
  wish to write JSON files as well as read them.
* [json65-file.h](src/json65-file.h) (1378 bytes) - Provides a helper
  function to feed data to the parser from a file, in chunks, and to
  display error messages to the user (including printing the offending
  line, and printing a caret to indicate the offending position of the
  line).

I hate build systems (or at least, build systems for C code), so I
have not provided one.  (Other than a lame little Perl script to build
and run the tests using [sim65][5].)  Instead, I encourage you to copy
the source files and header files you need into your own project, and
use whatever build system you are already using for your project.
(Such as the GNU Make based [cc65 build system][6].)

You can use the following dependency graph to determine which source
files you will need to copy into your project.  (For each source file,
you will also need to copy the corresponding header file.)  Source
files with no dependencies (such as `json65.s`) are at the top of the
graph, while the source file with the most dependencies
(`json65-print.c`) is at the bottom of the graph.

```
                json65.s    json65-string.s
                  /  \         /
                 /    \       /
                /      \     /
     json65-file.c    json65-tree.c     json65-quote.s
                           \             /
                            \           /
                             \         /
                            json65-print.c
```

If you wish to build and run the tests, simply run the `run-test.pl`
Perl script at the top level of the repository.  (It takes no
arguments.)  You'll need to have the [cc65][7] toolchain installed.

Note: version 2.17 and earlier of [sim65][5] have a
[bug in the implementation of the BIT instruction][8], so the tests
will fail.  You'll need a more recent version to get the tests to
pass.  (This only affects the simulation of the tests.  If you plan on
running JSON65 on real hardware, or on an emulator *other* than sim65,
then you'll be fine with an older version of cc65.)

## License

JSON65 is licensed under the [zlib/libpng license](LICENSE.txt), which
is approved by the [OSI][9] and the [FSF][10].

## Background

For more information about how and why I wrote JSON65, see [my blog post][11].

[1]: https://cc65.github.io/doc/ca65.html
[2]: https://cc65.github.io/doc/cc65-intern.html
[3]: https://tools.ietf.org/html/rfc8259#section-8.1
[4]: https://en.wikipedia.org/wiki/String_interning
[5]: https://cc65.github.io/doc/sim65.html
[6]: https://github.com/cc65/wiki/wiki/Bigger-Projects
[7]: https://cc65.github.io/cc65/
[8]: https://github.com/cc65/cc65/pull/712
[9]: https://opensource.org/licenses/Zlib
[10]: https://www.gnu.org/licenses/license-list.en.html#ZLib
[11]: https://funwithsoftware.org/posts/2021-03-03-json-on-the-apple-ii.html
