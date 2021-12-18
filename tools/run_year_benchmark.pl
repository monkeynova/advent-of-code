#!/usr/bin/env perl

use strict;
use warnings;

my ($year) = @ARGV;
if (!$year || $year < 2015 || $year > 2025) {
  die "USAGE:\n$0 <year>";
}

my $hostname = `hostname -s`;
my $json_out = "advent_of_code/${year}/benchmark/benchmark_${hostname}.json";
my $txt_out = "advent_of_code/${year}/benchmark/benchmark_${hostname}.txt";
chomp($hostname);
system("bazelisk run --config=benchmark advent_of_code/${year}:benchmark -- " .
       "--benchmark_out=`pwd`/${json_out} 2>&1 | tee ${txt_out}")
  and die "benchmark failed: ", $!;
