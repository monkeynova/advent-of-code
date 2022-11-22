#!/usr/bin/env perl

%find = map { $_ => 0 } ("namespace advent_of_code {\n",
                         "}  // namespace advent_of_code\n");

my ($fname) = @ARGV;

open my $ifh, '<', $fname or die;
open my $ofh, '>', $fname . ".new.$$" or die;
my $txt = join '', <$ifh>;
$txt =~ s[namespace advent_of_code {\s+namespace advent_of_code {]
         [namespace advent_of_code {];
$txt =~ s[}  // namespace advent_of_code\s+}  // namespace advent_of_code]
         [}  // namespace advent_of_code];
print {$ofh} $txt;
close $ofh or die;
close $ifh or die;
rename $fname . ".new.$$", $fname or die;