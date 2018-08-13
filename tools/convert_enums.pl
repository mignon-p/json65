#!/usr/bin/perl -w

my $inenum = 0;

sub hexify {
    my $arg = $_[0];
    $arg =~ s/0x/\$/;
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
        s/^\s+(\w+\s*)(=?\s*\w*),/"$1".hexify($2)/e;
        s%/\*(.*)\*/%;$1%;
        s/^\s*/        /;
        s/\s+$//;
        print $_, "\n";
    }
}
