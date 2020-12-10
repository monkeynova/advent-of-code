#!/usr/bin/env perl

use strict;
use warnings;

my @a = <>;
my %s = map { /Day(\d+)_(\d+)/ ? ($_ => $2 * 100 + $1) : () } @a;
@a = sort { ($s{$a} // 0) <=> ($s{$b} // 0) } @a;
print @a;
