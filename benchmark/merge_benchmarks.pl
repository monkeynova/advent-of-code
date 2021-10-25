#!/usr/bin/env perl

use strict;
use warnings;

my @lines = <>;
my %line_to_problem = map { /Day(\d+)_(\d+)/ ? ($_ => $2 * 100 + $1) : () } @lines;
@lines = sort { ($line_to_problem{$a} // 0) <=> ($line_to_problem{$b} // 0) } @lines;
my @column_sizes;
# Break columns on space except for "1235 ns".
my $col_split_re = '\s+(?!ns)';
for my $line (@lines) {
    chomp $line;
    my @cols = split /$col_split_re/, $line;
    for (my $i = 0; $i < @cols; ++$i) {
        $column_sizes[$i] //= 0;
        if ($column_sizes[$i] < length($cols[$i])) {
            $column_sizes[$i] = length($cols[$i]);
        }
    }
}
for my $line (@lines) {
    my @cols = split /$col_split_re/, $line;
    for (my $i = 0; $i < @cols; ++$i) {
        # First column is left-justified, the rest are right-justified.
        if ($i == 0) {
          $cols[$i] .= " " x ($column_sizes[$i] - length($cols[$i]));
        } else {
          $cols[$i] = (" " x ($column_sizes[$i] - length($cols[$i]))). $cols[$i];
        }
    }
    $line = join(" ", @cols) . "\n"
}
print @lines;
