#!/usr/bin/env perl

use strict;
use warnings;

my ($year, $day) = @ARGV;

die "USAGE:\n$0 <year> <day>\n" unless $year;
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
                  "s{/\\*year=\\*/-1}{/*year=*/${year}}g;",
                  "s{/\\*day=\\*/-1}{/*day=*/${url_day}}g;",
                  "s{New Day}{Day ${url_day}}g;",
                  "s{NewDay}{Day_${year}_${day}}g")),
        map { "advent_of_code/$year/day$day/$_" } values %renames)
  and die $!;

my $year_build_fname = "advent_of_code/${year}/BUILD";
my $build_contents = "";
if (! -f $year_build_fname) {
  $build_contents = <<EOF;
cc_library(
    name = "${year}",
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code/${year}/day${day}",
    ],
)
EOF
  open my $fh, '>', $year_build_fname
    or die "Can't write ${year_build_fname}: $!";
  print {$fh} $build_contents;
  close $fh;
  system("buildozer 'add deps //advent_of_code/$year' //advent_of_code/")
    and die $!;
} else {
  system("buildozer 'add deps //advent_of_code/$year:day$day' //advent_of_code/$year")
    and die $!;
}

# Non-fatal if clang-format isn't present.
system("find advent_of_code/$year/day$day -name '*.h' -o -name '*.cc' | " .
       "xargs clang-format --style=Google -i");

# Non-fatal if buildifier isn't present.
system("find advent_of_code/$year/day$day -name BUILD | xargs buildifier");
system("buildifier advent_of_code/$year/BUILD");

system('perl', '-spi', '-e', 's/CURRENT_YEAR="(.*)"/CURRENT_YEAR="$year"/g', 'BUILD');
system('perl', '-spi', '-e', 's/CURRENT_DAY="(.*)"/CURRENT_DAY="$day"/g', 'BUILD');
