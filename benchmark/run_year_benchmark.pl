#!/usr/bin/env perl

use strict;
use warnings;
use JSON;

my ($year) = @ARGV;
if (!$year || $year < 2015 || $year > 2025) {
  die "USAGE:\n$0 <year>";
}

my $hostname = `hostname -s`;
chomp($hostname);
my $tmp_file = "/tmp/benchmark_out.txt.$$";
my $tmp_json_file = "/tmp/benchmark_out.json.$$";
my $subpath = "advent_of_code/${year}";
my $bazel_targets = "${subpath}/...";

{
  open my $fh, '>', $tmp_file or die "Cannot make tmp file ${tmp_file}";
  close $fh;
}
my @targets =
  split /\n/, `bazelisk query 'attr("tags", "benchmark", '${bazel_targets}')'`;
my $merged_json;
for my $t (@targets) {
  system("bazelisk run -c opt ${t} -- " .
         "--benchmark --benchmark_out=${tmp_json_file} " .
         "--run_long_tests=1m | " .
         "grep BM_ >> ${tmp_file}")
    and die $!;
  open my $fh, '<', $tmp_json_file or die "Cannot read ", $tmp_json_file;
  my $tmp_json = decode_json(join '', <$fh>) or die "Bad json";
  $merged_json = merge_json($merged_json, $tmp_json);
}
{
  open my $fh, '>', "${subpath}/benchmark/benchmark_${hostname}.json"
    or die "Cannot write to merged json";
  print {$fh} JSON->new->utf8(1)->canonical(1)->pretty(1)->encode($merged_json);
  close $fh;
}
system("benchmark/merge_benchmarks.pl < ${tmp_file} " .
       "> ${subpath}/benchmark/benchmark_${hostname}.txt")
  and die $!;

sub merge_json {
  my ($in_merged, $new) = @_;
  if (!$in_merged) { return $new; }
  scalar keys %$in_merged;
  while (my ($k, $v) = each %$in_merged) {
    if (ref($v) eq 'ARRAY') {
      push @$v, @{$new->{$k}};
    }
    delete $new->{$k};
  }
  for my $k (keys %$new) {
    $in_merged->{$k} = $new->{$k};
  }

  return $in_merged;
}