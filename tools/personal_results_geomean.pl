#!/usr/bin/env perl

use strict;
use warnings;

my ($fname) = @ARGV;
die "USAGE\n$0 <fname>\n" unless $fname;

open my $fh, '<', $fname or die "Can't open $fname";

my $header;
my $geo_mean1 = 0;
my $geo_mean2 = 0;
my $count = 0;
while (<$fh>) { 
    if (/^Day/) {
        $header = $_;
    } elsif ($header) {
        my (undef, $day, $t1, $r1, $s1, $t2, $r2, $s2) = split /\s+/;
        ++$count;
        $geo_mean1 += log($r1);
        $geo_mean2 += log($r2);
    }
}
$geo_mean1 /= $count;
$geo_mean2 /= $count;
$geo_mean1 = exp($geo_mean1);
$geo_mean2 = exp($geo_mean2);

my $out = " " x length($header);
substr($out, 0, 3) = "GEO";
my $r1_pos = index($header, "Rank", 0) + 4 - length(int($geo_mean1));
substr($out, $r1_pos, length(int($geo_mean1))) = int($geo_mean1);
my $r2_pos = index($header, "Rank", $r1_pos + 1) + 4 - length(int($geo_mean2));
substr($out, $r2_pos, length(int($geo_mean2))) = int($geo_mean2);
print $out, "\n";


