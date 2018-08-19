#!/usr/bin/perl -w

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
