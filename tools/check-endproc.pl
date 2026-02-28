#!/usr/bin/perl -w

# Script to check that the comment on the ".endproc" line matches
# the name on the corresponding ".proc" line.

use strict;

my $name     = undef;
my $procLine = undef;
my $exitCode = 0;

while (<>) {
    chomp;

    if (/^\.proc\s+(\S+)/) {
        $name     = $1;
        $procLine = $.;
    } elsif (/^.endproc/) {
        if (/;\s*(\S+)/) {
            my $endName = $1;

            if ($name ne $endName) {
                print STDERR
                    "$ARGV: $endName on line $. but ",
                    "$name on line $procLine\n";
                $exitCode = 1;
            }
        } else {
            print STDERR
                "$ARGV: no name on line $. but ",
                "$name on line $procLine\n";
            $exitCode = 1;
        }
    }
} continue {
    close ARGV if eof;  # Not eof()!
}

exit ($exitCode);
