#!/usr/bin/env perl

use strict;
use warnings;

use HTML::FormatText;
use HTML::TreeBuilder;
use HTTP::Cookies;
use LWP::UserAgent;

die "AOC_SESSION environment variable not set" unless $ENV{AOC_SESSION};

die "USAGE\n$0 <year> <day>" if @ARGV < 2;

my ($year, $day) = @ARGV;

die "Bad year: $year" unless $year =~ /^\d+$/ && $year >= 2015;
die "Bad day: $day" unless $day =~ /^\d+$/ && $day > 0 && $day <= 25;

my $input_url = sprintf("https://adventofcode.com/%4d/day/%d", $year, $day);
my $out_file = sprintf("advent_of_code/%4d/day%02d/day%02d.cc", $year, $day, $day);

print "Fetching $input_url into $out_file ...\n";
my $cookies = HTTP::Cookies->new();
$cookies->set_cookie(
    1,                  # version
    'session',          # key
    $ENV{AOC_SESSION},  # value
    '/',                # path
    'adventofcode.com', # domain
    443,                # port
    undef,              # path_spec
    1,                  # secure
    10,                 # max_age (seconds)
    undef,              # discard
);
my $ua = LWP::UserAgent->new(cookie_jar => $cookies);
my $response = $ua->get($input_url);
die $response->status_line unless $response->is_success;

# Euler Project puts the title of the problem in the only h2 block on the page.
if ($response->decoded_content !~ m{<h2>(.*?)</h2>}s) {
  die "Can't find problem title (in h2)";
}
my $title = $1;

my ($part1_html, $part2_html) = 
  $response->decoded_content =~ m{<article[^>]+class="day-desc"[^>]*>(.*?)</article>}sg;
$part1_html or die "Can't find Part1";
$part2_html or die "Can't find Part2";

my $part1_txt =
    HTML::FormatText
        ->new()
        ->format(HTML::TreeBuilder->new->parse($part1_html));
$part1_txt =~ s/\s+$//;
$part1_txt =~ s{(^|\n) *}{$1// }g;

my $part2_txt =
    HTML::FormatText
        ->new()
        ->format(HTML::TreeBuilder->new->parse($part2_html));
$part2_txt =~ s/\s+$//;
$part2_txt =~ s{(^|\n) *}{$1// }g;

my $header_comment = "// $input_url\n//\n$part1_txt\n//\n$part2_txt\n\n";

open my $ifh, '<', $out_file or die "Can't read $out_file: $!";
open my $ofh, '>', "$out_file.new.$$" or die "Can't write $out_file (tmp): $!";
print {$ofh} $header_comment;
my $in_header = 1;
while (<$ifh>) {
    next if $in_header && m{^//};
    $in_header = 0;
    print {$ofh} $_;
}
close $ofh or die "Can't close $out_file (tmp): $!";
close $ifh or die "Can't close $out_file: $!";
rename "$out_file.new.$$", $out_file or die "Can't rename $out_file: $!";