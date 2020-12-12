#!/usr/bin/env perl

use strict;
use warnings;

my ($year, $day) = @ARGV;

die "Bad year: $year" unless $year >= 2015;
die "Bad day: $day" unless $day > 0 && $day <= 25;

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
       join(" ", ("s{new_day_path}{$year/day$day}g;",
                  "s{new_day}{day$day}g;",
                  "s{NEW_DAY}{${year}_DAY$day}g;",
                  "s{NewDay}{Day${day}_$year}g")),
        map { "advent_of_code/$year/day$day/$_" } values %renames)
  and die $!;
