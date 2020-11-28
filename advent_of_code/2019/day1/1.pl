#!/usr/bin/env perl

use warnings;
use strict;

my $sum = 0;

while (<>) {
    $sum += int($_ /3) - 2;
}

print $sum, "\n";
