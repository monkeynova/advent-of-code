#!/usr/bin/env perl

use strict;
use warnings;

my ($year, $day) = @ARGV;

die "USAGE:\n$0 <year> <day>" if !$year || !$day;

my $path = "advent_of_code/${year}/day${day}";
die "Bad path: ${path}\nUSAGE:\n$0 <year> <day>" if ! -d $path;

{
    open my $ifh, '<', "$path/day${day}.cc" or die;
    open my $ofh, '>', "$path/day${day}.cc.$$.new" or die;
    print {$ofh} "// http://adventofcode.com/$year/day/$day\n\n";
    while (<$ifh>) { print {$ofh} $_; }
    close $ifh;
    close $ofh;
    rename "$path/day${day}.cc.$$.new", "$path/day${day}.cc" or die;
}
{
    open my $ifh, '<', "$path/day${day}.test" or die;
    open my $ofh, '>', "$path/day${day}.test.$$.new" or die;
    print {$ofh} "# http://adventofcode.com/$year/day/$day\n==\n\n";
    while (<$ifh>) { print {$ofh} $_; }
    close $ifh;
    close $ofh;
    rename "$path/day${day}.test.$$.new", "$path/day${day}.test" or die;
}
{
    open my $ifh, '<', "$path/day${day}_benchmark.cc" or die;
    open my $ofh, '>', "$path/day${day}_benchmark.cc.$$.new" or die;
    while (<$ifh>) {
      s{constexpr int kMaxTestCount = (\d+);}
       {"constexpr int kMaxTestCount = " . ($1 + 1) . ";"}e;
      print {$ofh} $_;
    }
    close $ifh;
    close $ofh;
    rename "$path/day${day}_benchmark.cc.$$.new", "$path/day${day}_benchmark.cc" or die;
}