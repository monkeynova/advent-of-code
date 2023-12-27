#!/usr/bin/perl

use strict;
use warnings;

my ($dir) = (@ARGV);
die unless $dir;
die unless $dir =~ m,/(day\d{2})$,;
my $day = $1;

open my $fh, '<', "$dir/README.md";
my $title = <$fh>;
$title =~ s/^\#\# // or die;
$title =~ s/ \#\#$// or die;
close $fh;

undef $/;

open $fh, '<', "$dir/${day}.h" or die;
my $h_insert = <<EOF;
  virtual std::string_view test_file() const override {
    return "$title";
  }
EOF
my $contents = <$fh>;
$contents =~ s/^  virtual std::string_view test_file() const override {$/$h_insert\n$1/ or die;
close $fh or die;
open $fh, '>', "$dir/${day}.h" or die;
print {$fh} $contents;
close $fh or die;
