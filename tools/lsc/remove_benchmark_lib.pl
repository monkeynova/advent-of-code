#!/usr/bin/perl

use strict;
use warnings;

my ($fname) = @ARGV;
die unless $fname;
die "bad file: $fname" unless $fname =~ m,/(\d{4})/day(\d{2})/BUILD,;
my ($year, $day) = ($1, $2);

open my $fh, '<', $fname or die "Can't open $fname: $!";
undef $/;
my $contents = <$fh>;
close $fh or die "Can't close $fname: $!";

$contents =~ m,cc_library\(\s*name = "day${day}"[^\)]*deps = \[([^\]]*)\], or die "No deps";
my $deps = $1;

open $fh, '>', $fname or die "Can't write $fname: $!";
print {$fh} <<EOF;
load("\@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day${day}",
    srcs = ["day${day}.cc"],
    hdrs = ["day${day}.h"],
    data = [
        "input.txt",
    ],
    visibility = [
        "//visibility:public",
    ],
    alwayslink = 1,
    deps = [$deps],
)

cc_test(
    name = "day${day}_test",
    size = "small",
    srcs = ["day${day}_test.cc"],
    data = [
        "day${day}.test",
    ],
    deps = [
        ":day${day}",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day${day}_benchmark",
    srcs = ["day${day}_benchmark.cc"],
    size = "small",
    tags = ["benchmark"],
    data = [
        "day${day}.test",
    ],
    deps = [
        ":day${day}",
        "//advent_of_code/infra:file_benchmark",
        "\@com_monkeynova_gunit_main//:test_main",
    ],
)
EOF
close $fh or die "Can't close $fname: $!";