#!/usr/bin/env perl

use strict;
use warnings;

my ($year) = @ARGV;
if (!$year || $year < 2015 || $year > 2025) {
  die "USAGE:\n$0 <year>";
}

my $hostname = `hostname -s`;
chomp($hostname);
system("bazelisk run -c opt advent_of_code/${year}:benchmark -- " .
       "--benchmark " .
       "--benchmark_out=`pwd`/advent_of_code/${year}/benchmark/benchmark_${hostname}.json " .
       "--run_long_tests=1m --advent_day_run_audit=false " .
       "2>&1 | tee advent_of_code/${year}/benchmark/benchmark_${hostname}.txt")
  and die "benchmark failed: ", $!;
