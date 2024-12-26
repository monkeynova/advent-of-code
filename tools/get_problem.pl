#!/usr/bin/env perl

use strict;
use warnings;

use HTML::FormatText;
use HTML::FormatMarkdown;
use HTML::TreeBuilder;
use HTTP::Cookies;
use LWP::UserAgent;

die "AOC_SESSION environment variable not set" unless $ENV{AOC_SESSION};

my ($year, $day);
open my $fh, '<', 'BUILD'
    or die "Cannot open BUILD to extract current year and day";
while (<$fh>) {
    $year = $1 if /CURRENT_YEAR="(.*)"/;
    $day = $1 if /CURRENT_DAY="(.*)"/;
}
close $fh;

die "Could not extract year from BUILD" unless $year;
die "Could not extract day from BUILD" unless $day;

die "Bad year: $year" unless $year =~ /^\d+$/ && $year >= 2015;
die "Bad day: $day" unless $day =~ /^\d+$/ && $day > 0 && $day <= 25;

my $input_url = sprintf("https://adventofcode.com/%4d/day/%d", $year, $day);
my $out_file = sprintf("advent_of_code/%4d/day%02d/README.md", $year, $day);

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

my $part1_md =
    HTML::FormatMarkdown
        ->new()
        ->format(HTML::TreeBuilder->new->parse($part1_html));
$part1_md =~ s/`\*(\d+)\*`/*`$1`*/g;

my $part2_md =
    HTML::FormatMarkdown
        ->new()
        ->format(HTML::TreeBuilder->new->parse($part2_html));
$part2_md =~ s/`\*(\d+)\*`/*`$1`*/g;

open my $ofh, '>', $out_file or die "Can't write to $out_file: $!";
print {$ofh} $part1_md;
print {$ofh} "\n";
print {$ofh} $part2_md;
close $ofh or die "Error writing to $out_file: $!";

system("perl", "tools/add_title.pl", sprintf("advent_of_code/%4d/day%02d", $year, $day));