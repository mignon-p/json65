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

use FindBin;
my $root = $FindBin::Bin;

chdir ($root);

my $src = "src";
my $test = "tests";
my $example = "examples";

my $blue = "\e[34m";
my $green = "\e[32m";
my $red = "\e[31m";
my $off = "\e[0m";

my %test_results = ();

my $total_bytes = 0;

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

sub mysystem_nonfatal {
    my @cmd = @_;
    print join(" ", @cmd), "\n";
    if (system (@cmd) != 0) {
        if ($? == -1) {
            die "$red*** fatal: $!$off\n";
        } elsif ($? & 127) {
            die (sprintf ("$red*** fatal: signal %d$off\n", $? & 127));
        } else {
            return "fail";
        }
    }
    return "pass";
}

sub print_heading {
    my $str = $_[0];
    print "$blue*** $str$off\n";
}

sub build_program {
    my ($hash, @sources) = @_;

    my $prog = $hash->{"prog"};
    my $map = $prog . ".map";
    my $target = "sim6502";
    $target = $hash->{"target"} if (exists $hash->{"target"});

    my @cmd = ("cl65", "-I$src", "-W-unused-param", "-O");
    push @cmd, '-t', $target;
    push @cmd, '-C', $hash->{"config"} if (exists $hash->{"config"});
    push @cmd, '-o', $prog;
    push @cmd, '-m', $map;
    push @cmd, @sources;

    mysystem (@cmd);
}

sub run_test {
    my $t = $_[0];

    print_heading "Running $t";
    $test_results{$t} = mysystem_nonfatal ("sim65", $t);
}

sub parse_map {
    my $map = $_[0];
    my $name = "";
    my $size = 0;
    my %result = ();

    open F, $map or die;
    while (<F>) {
        chomp;
        if (/^([^:\s]+):$/) {
            $name = $1;
            $size = 0;
        } elsif (/^\s+[A-Z].* Size\=(\w+)/) {
            my $sz = hex ($1);
            $size += $sz;
            $result{$name} = $size;
        } elsif (/^$/) {
            last;
        }
    }
    close F;

    return \%result;
}

sub print_size {
    my ($sizes, $obj) = @_;
    my $size = $sizes->{$obj};

    printf "%-15s %4u bytes\n", $obj, $size;
    $total_bytes += $size;
}

print_heading "Building tests";

# Tests which can be built for sim65
build_program({'prog' => "$test/test"},
              "$src/json65.s", "$test/test.c");
build_program({'prog' => "$test/test-string"},
              "$src/json65-string.s", "$test/test-string.c");
build_program({'prog' => "$test/test-tree"},
              "$src/json65.s", "$src/json65-string.s", "$src/json65-tree.c",
              "$test/test-tree.c");
build_program({'prog' => "$test/test-quote"},
              "$src/json65-quote.s", "$test/test-quote.c");
build_program({'prog' => "$test/test-print"},
              "$src/json65.s", "$src/json65-string.s", "$src/json65-tree.c",
              "$src/json65-quote.s", "$src/json65-print.c",
              "$test/test-print.c");

# test-file uses library functions (ftell and fseek) which are not available
# on sim65, so we build it for Apple II instead.  This means that we cannot
# test it automatically, though.  (But it's still worth building, to make
# sure it builds.)
build_program({'prog' => "$test/testfile.system",
               'target' => 'apple2',
               'config' => 'apple2-system.cfg'},
              "$src/json65.s", "$src/json65-file.c", "$test/test-file.c");
build_program({'prog' => "$example/example.system",
               'target' => 'apple2',
               'config' => 'apple2-system.cfg'},
              "$src/json65.s", "$src/json65-file.c",
              "$src/json65-string.s", "$src/json65-tree.c",
              "$src/json65-quote.s", "$src/json65-print.c",
              "$example/example.c");

chdir ($test);

# Run tests on sim65
run_test ("test");
run_test ("test-string");
run_test ("test-tree");
run_test ("test-print");

# test-quote is not self-checking, and its functionality is subsumed
# by test-print, so there's no need to run it
#run_test ("test-quote");

print_heading "Size summary";

my $print_map = parse_map ("test-print.map");
my $file_map = parse_map ("testfile.system.map");

print_size ($print_map, "json65.o");
print_size ($print_map, "json65-string.o");
print_size ($print_map, "json65-tree.o");
print_size ($print_map, "json65-quote.o");
print_size ($print_map, "json65-print.o");
print_size ($file_map,  "json65-file.o");
printf "%-15s %4u bytes\n", "total", $total_bytes;

my $failures = 0;

print_heading "Test summary";
foreach my $t (sort keys %test_results) {
    printf "%-11s ", $t;
    if ($test_results{$t} eq "pass") {
        print $green, "PASS", $off, "\n";
    } else {
        print $red, "FAIL", $off, "\n";
        $failures++;
    }
}

exit $failures;
