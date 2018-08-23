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
