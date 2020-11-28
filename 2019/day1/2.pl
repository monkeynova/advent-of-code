#!/usr/bin/env perl

use strict;
use warnings;

my $sum = 0;

while (<>) {
    my $fuel = int($_ /3) - 2;
    my $ff = $fuel;
    while (int($ff / 3) - 2 > 0) {
         $ff = int($ff / 3) - 2;
         $fuel += $ff;
    }
    $sum += $fuel
}

print $sum, "\n"
