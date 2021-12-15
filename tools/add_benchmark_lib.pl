#!/usr/bin/env perl

use strict;
use warnings;

my ($year, $day) = @ARGV;

die "USAGE:\n$0 <year> <day>" if !$year || !$day;

my $path = "advent_of_code/${year}/day${day}";
die "Bad path: ${path}\nUSAGE:\n$0 <year> <day>" if ! -d $path;

{
  my $fname = "${path}/BUILD";
  my $data = read_fully($fname);
  if ($data !~ /day${day}_benchmark_lib/) {
    print "Updating ${fname} ...\n";
    my $impl = <<EOF;
cc_library(
    name = "day${day}_benchmark_lib",
    srcs = ["day${day}_benchmark.cc"],
    testonly = 1,
    alwayslink = 1,
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":day${day}_input",
        "//advent_of_code/infra:file_benchmark",
        "\@com_monkeynova_gunit_main//:test_main",
    ],
)

cc_test(
    name = "day${day}_benchmark",
    tags = ["benchmark"],
    deps = [
        ":day${day}_benchmark_lib",
        "\@com_monkeynova_gunit_main//:test_main",
    ],
)
EOF
    $data =~ s/cc_test\([^\)]+"day${day}_benchmark"[^\)]+\)/$impl/;
    open my $fh, '>', $fname or die "cannot write to ", $fname;
    print {$fh} $data;
    close($fh);
  }
}

{
  my $fname = "${path}/day${day}_benchmark.cc";
  my $data = read_fully($fname);
  if ($data =~ /DayBenchmarkTest, Consistency/) {
    print "Updating ${fname} ...\n";
    my $impl = <<EOF;
DayBenchmarkTest_${year}_${day}, Consistency
EOF
    $data =~ s/DayBenchmarkTest, Consistency/$impl/;
    open my $fh, '>', $fname or die "cannot write to ", $fname;
    print {$fh} $data;
    close($fh);
  }
}


sub read_fully {
  my ($fname) = @_;
  open my $fh, '<', $fname or die "cannot open ", $fname;
  my $data = join '', <$fh>;
  close($fh);
  return $data;
}