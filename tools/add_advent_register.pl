#!/usr/bin/perl

use strict;
use warnings;

my ($fname) = @ARGV;
$fname or die;
$fname =~ m,/(\d{4})/day(\d{2})/, or die;
my ($year, $day) = (int($1), int($2));

open my $ifh, '<', $fname or die "can't open $fname: $!";
my $tmp_fname = $fname . '.new.' . $$;
open my $ofh, '>', $tmp_fname or die "can't open $tmp_fname: $!";
my $saw_registry;
while (<$ifh>) {
    $saw_registry ||= /RegisterAdventDay/;
    if (m,\}  // namespace advent_of_code, && !$saw_registry) {
        print {$ofh} <<EOF;
static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/$year, /*day=*/$day, []() {
  return std::unique_ptr<AdventDay>(new Day_${year}_${day}());
});

EOF
    }
    print {$ofh} $_;
}
close $ofh or die "Can't close: $tmp_fname: $!";
close $ifh or die "Can't close: $fname: $!";
rename $tmp_fname, $fname or die "Can't rename $tmp_fname to $fname: $!";