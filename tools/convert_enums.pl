#!/usr/bin/perl -w

# JSON65 - A JSON parser for the 6502 microprocessor.
#
# https://github.com/ppelleti/json65
#
# Copyright © 2018 Patrick Pelletier
#
# This software is provided 'as-is', without any express or implied
# warranty.  In no event will the authors be held liable for any damages
# arising from the use of this software.
#
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely, subject to the following restrictions:
#
# 1. The origin of this software must not be misrepresented; you must not
#    claim that you wrote the original software. If you use this software
#    in a product, an acknowledgment in the product documentation would be
#    appreciated but is not required.
# 2. Altered source versions must be plainly marked as such, and must not be
#    misrepresented as being the original software.
# 3. This notice may not be removed or altered from any source distribution.

my $inenum = 0;

sub hexify {
    my $arg = $_[0];
    $arg =~ s/0x/\$/;
    $arg =~ s/(-\d+)/sprintf "\$%02x", $1 & 0xff/e;
    return $arg;
}

while (<>) {
    chomp;
    if (/^enum\s+(\w+)/) {
        print ";; $1\n";
        print ".enum\n";
        $inenum = 1;
    } elsif (/^\}\;/) {
        print ".endenum\n\n";
        $inenum = 0;
    } elsif ($inenum) {
        s/^\s+(\w+\s*)(=?\s*[-\w]*),/"$1".hexify($2)/e;
        s%/\*(.*)\*/%;$1%;
        s/^\s*/        /;
        s/\s+$//;
        print $_, "\n";
    }
}
