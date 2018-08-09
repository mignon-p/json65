#!/usr/bin/perl -w

my ($ws, $str, $lit, $int, $num) =
    qw(prop_ws prop_str prop_lit prop_int prop_num);
my ($lsq, $lcur, $rsq, $rcur, $colon, $comma, $quote) =
    qw(sc_lsq sc_lcur sc_rsq sc_rcur sc_colon sc_comma sc_quote);

my @props = ();

for (my $i = 0; $i < 128; $i++) {
    push @props, [];
}

foreach my $c (0x20, 0x09, 0x0a, 0x0d) {
    push $props[$c], $ws;
}

for (my $c = 32; $c < 128; $c++) {
    push $props[$c], $str;
}

foreach my $c (split (//, "aeflnrstu")) {
    push $props[ord($c)], $lit;
}

foreach my $c (split (//, "-0123456789")) {
    push $props[ord($c)], $int;
    push $props[ord($c)], $num;
}

foreach my $c (split (//, ".eE+")) {
    push $props[ord($c)], $num;
}

push $props[ord("[")], $lsq;
push $props[ord("{")], $lcur;
push $props[ord("]")], $rsq;
push $props[ord("}")], $rcur;
push $props[ord(":")], $colon;
push $props[ord(",")], $comma;
push $props[ord("\"")], $quote;

print "charprops:\n";
my $c = 0;
foreach my $prop (@props) {
    print "        .byte ";
    my $x = join ("|", @$prop);
    if ($x eq "") {
        $x = "0";
    }
    printf "%-27s", $x;
    print "; ";
    if ($c > 32 and $c < 127) {
        print chr($c);
    } else {
        printf "%s%02x", '$', $c;
    }
    print "\n";
    $c++;
}
