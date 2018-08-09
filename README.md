I was watching TV, and there was a commercial which proclaimed, "It's
time to do what *you* want!"  I replied to the TV, "It's time to write
a JSON parser in 6502 assembly language?"  Somehow I don't think
that's what they had in mind, but the TV is right, I *should* do what
I want.

So, here is my JSON parser.  The core parser is written entirely in
6502 assembly language, and is meant to be assembled with [ca65][1].
However, it is meant to be called from C, and uses the
[cc65 calling convention][2] (specifically the `fastcall` convention).

JSON65 is an event-driven (SAX-style) parser, so the parser is given a
callback function, which it calls for each event.

JSON65 supports incremental parsing, so you can freely feed it any
sized chunks of input, and you don't need to have the whole file in
memory at once.

JSON65 is fully reentrant, so you can incrementally parse several
files at once if you so desire.

JSON65 does have a couple of limits: strings are limited to 255 bytes,
and the nesting depth (of nested arrays or objects) is limited to 245.
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
or CRLF.)

JSON65 will parse numbers which fit into a 32-bit signed long, and
will provide the long to the callback.  All other numbers
(i. e. floating point numbers, or integers which overflow a 32-bit
long) are provided to the callback as a string.  (Like strings,
numbers cannot be more than 255 digits long.)


[1]: https://cc65.github.io/doc/ca65.html
[2]: https://cc65.github.io/doc/cc65-intern.html
[3]: https://tools.ietf.org/html/rfc8259#section-8.1
