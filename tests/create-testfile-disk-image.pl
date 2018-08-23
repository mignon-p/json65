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

use strict;
use FindBin;

# Assumes that the "ac" shell script wrapper for Apple Commander
# is on your PATH:
# https://applecommander.github.io/install/

my $ac = "ac";

my $diskimage = "testfile.po";

my $tests = $FindBin::Bin;
chdir ($tests);

my $blue = "\e[34m";
my $green = "\e[32m";
my $red = "\e[31m";
my $off = "\e[0m";

sub mysystem {
    my @cmd = @_;
    print join(" ", @cmd), "\n";
    if (system (@cmd) != 0) {
        if ($? == -1) {
            die "$red*** fatal: $!$off\n";
        } elsif ($? & 127) {
            die (sprintf ("$red*** fatal: signal %d$off\n", $? & 127));
        } else {
            die (sprintf ("$red*** fatal: exit code %d$off\n", $? >> 8));
        }
    }
}

sub print_heading {
    my $str = $_[0];
    print "$blue*** $str$off\n";
}

print_heading "Creating disk image";
mysystem ("rm", "-f", $diskimage);
mysystem ($ac, "-pro140", $diskimage, "testfile");

print_heading "Adding test program";
my $program = "testfile.system";
mysystem ("$ac -as $diskimage $program < $program");

print_heading "Adding JSON files";
my @json = split (' ', `echo file??.json`);
foreach my $json (@json) {
    mysystem ("$ac -p $diskimage $json TXT < $json");
}
