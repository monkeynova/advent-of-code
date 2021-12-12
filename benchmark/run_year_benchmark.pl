#!/usr/bin/env perl

use strict;
use warnings;

my ($year) = @ARGV;
if (!$year || $year < 2015 || $year > 2025) {
  die "USAGE:\n$0 <year>";
}

my $tmp_file = "/tmp/benchmark_out.txt.$$";
my $subpath = "advent_of_code/${year}";
my $bazel_targets = "${subpath}/...";

open my $fh, '>', $tmp_file or die "Cannot make tmp file ${tmp_file}";
close $fh;
my @targets =
  split /\n/, `bazelisk query 'attr("tags", "benchmark", '${bazel_targets}')'`;
for my $t (@targets) {
  system("bazelisk run -c opt ${t} -- --benchmark --run_long_tests=1m | " .
         "grep BM_ >> ${tmp_file}")
    and die $!;
}
system("benchmark/merge_benchmarks.pl < ${tmp_file} " .
       "> ${subpath}/benchmark/benchmark_`hostname -s`.txt")
  and die $!;
