#!/usr/bin/perl -w

use strict;

my @schars =
    qw(dt_none dt_lsq dt_lcur dt_rsq dt_rcur dt_colon dt_comma dt_quote);

my ($none, $lsq, $lcur, $rsq, $rcur, $colon, $comma, $quote) = @schars;

my ($rdy, $rdy_ca, $key, $key_co, $ncolon, $ncomma_ca, $ncomma_co, $done) =
    (0, 1, 2, 3, 4, 5, 6, 7);

my ($perr, $ichar) =
    qw(disp_parse_error disp_illegal_char);
my ($xstr, $xcolon, $xcomma) =
    qw(disp_exp_string disp_exp_colon disp_exp_comma);
my ($xobj, $xarr) =
    qw(disp_exp_obj_end disp_exp_array_end);

my ($sobj, $eobj) =
    qw(disp_start_obj disp_end_obj);
my ($sarr, $earr, $sstr) =
    qw(disp_start_array disp_end_array disp_start_string);
my ($comarr, $comobj, $dcolon) =
    qw(disp_comma_array disp_comma_object disp_colon);

my %table = ();

my @default = ($perr, $perr, $perr, $perr, $perr, $perr, $perr, $perr);
$default[$key] = $xstr;
$default[$key_co] = $xstr;
$default[$ncolon] = $xcolon;
$default[$ncomma_ca] = $xcomma;
$default[$ncomma_co] = $xcomma;

foreach my $schar (@schars) {
    $table{$schar} = [@default];
}

$table{$none} =
    [$ichar, $ichar, $ichar, $ichar, $ichar, $ichar, $ichar, $ichar];

foreach my $state ($rdy_ca, $ncomma_ca) {
    $table{$rcur}[$state] = $xarr;
}

foreach my $state ($key_co, $ncomma_co) {
    $table{$rsq}[$state] = $xobj;
}

foreach my $state ($rdy, $rdy_ca) {
    $table{$lsq}[$state]   = $sarr;
    $table{$lcur}[$state]  = $sobj;
    $table{$quote}[$state] = $sstr;
}

$table{$rsq}[$rdy_ca]   = $earr;
$table{$quote}[$key] = $sstr;
$table{$quote}[$key_co] = $sstr;
$table{$rcur}[$key_co]  = $eobj;
$table{$colon}[$ncolon] = $dcolon;
$table{$comma}[$ncomma_ca] = $comarr;
$table{$comma}[$ncomma_co] = $comobj;
$table{$rsq}[$ncomma_ca] = $earr;
$table{$rcur}[$ncomma_co] = $eobj;

foreach my $schar (@schars) {
    printf ".define %-8s", $schar;
    my $sep = " ";
    foreach my $func (@{$table{$schar}}) {
        print $sep, $func, "-1";
        $sep = ",";
    }
    print "\n";
}
