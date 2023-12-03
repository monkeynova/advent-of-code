#!/usr/bin/env perl

use strict;
use warnings;

my ($year, $day) = @ARGV;

die "Bad year: $year" unless $year >= 2015;
die "Bad day: $day" unless $day > 0 && $day <= 25;

$day = sprintf "%02d", $day;
my $url_day = sprintf "%d", $day;

mkdir "advent_of_code/$year" if ! -d "advent_of_code/$year";
die "advent_of_code/$year/day$day already exists" if -d "advent_of_code/$year/day$day";
system("cp", "-r", "advent_of_code/new_day_path", "advent_of_code/$year/day$day")
  and die $!;

opendir my $dir, "advent_of_code/$year/day$day" or die $!;
my %renames;
while (my $fname = readdir($dir)) {
    next if -d "advent_of_code/$year/day$day/$fname";
    $renames{$fname} = $fname;
    $renames{$fname} =~ s{new_day}{day$day};
}
closedir $dir or die $!;

while (my ($src, $dest) = each %renames) {
    if ($src ne $dest) {
        rename "advent_of_code/$year/day$day/$src",
               "advent_of_code/$year/day$day/$dest" or die $!;
    }
}

system("perl", "-spi", "-e",
       join(" ", ("s{^// clang-format.*\n}{}g;",
                  "s{new_day_path}{$year/day$day}g;",
                  "s{new_day_url}{$year/day/$url_day}g;",
                  "s{new_day}{day$day}g;",
                  "s{NEW_DAY}{${year}_DAY$day}g;",
                  "s{NewDay}{Day_${year}_${day}}g")),
        map { "advent_of_code/$year/day$day/$_" } values %renames)
  and die $!;

my $year_build_fname = "advent_of_code/${year}/BUILD";
my $build_contents = "";
if (! -f $year_build_fname) {
  $build_contents = <<EOF;
cc_test(
    name = "benchmark",
    size = "small",
    tags = ["benchmark"],
    deps = [
        ":benchmark_lib",
        "\@com_monkeynova_gunit_main//:test_main",
    ],
)

sh_binary(
    name = "whole_year",
    testonly = 1,
    srcs = ["//tools:whole_year.sh"],
    data = [":benchmark"],
    args = ["$(location :benchmark)"],
)

cc_library(
    name = "benchmark_lib",
    testonly = 1,
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code/${year}/day${day}:day${day}_benchmark_lib",
    ],
)
EOF
} else {
  open my $fh, '<', $year_build_fname;
  $build_contents = join '', <$fh>;
  close $fh;
  $build_contents =~ s{
    (cc_library\([^\)]+
     name\s*=\s*"benchmark_lib"[^\)]+)
    ("//advent_of_code/\d+/day\d+:day\d+_benchmark_lib",[^\)]+\))
  }{$1
    "//advent_of_code/${year}/day${day}:day${day}_benchmark_lib",
    $2
  }x or die "Could not add benchmark to the year target";
}
open my $fh, '>', $year_build_fname
  or die "Can't write ${year_build_fname}: $!";
print {$fh} $build_contents;
close $fh;


# Non-fatal if clang-format isn't present.
system("find advent_of_code/$year/day$day -name '*.h' -o -name '*.cc' | " .
       "xargs clang-format --style=Google -i");

# Non-fatal if buildifier isn't present.
system("find advent_of_code/$year/day$day -name BUILD | xargs buildifier");
system("buildifier advent_of_code/$year/BUILD");