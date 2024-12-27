#!/usr/bin/env perl

use strict;
use warnings;

use HTTP::Cookies;
use LWP::UserAgent;

die "AOC_SESSION environment variable not set" unless $ENV{AOC_SESSION};

my ($year, $day);
open my $fh, '<', 'BUILD'
    or die "Cannot open BUILD to extract current year and day";
while (<$fh>) {
    $year = $1 if /CURRENT_YEAR = "(.*)"/;
    $day = $1 if /CURRENT_DAY = "(.*)"/;
}
close $fh;

die "Could not extract year from BUILD" unless $year;
die "Could not extract day from BUILD" unless $day;

die "Bad year: $year" unless $year =~ /^\d+$/ && $year >= 2015;
die "Bad day: $day" unless $day =~ /^\d+$/ && $day > 0 && $day <= 25;

my $input_url = sprintf("https://adventofcode.com/%4d/day/%d/input", $year, $day);
my $out_file = sprintf("advent_of_code/%4d/day%02d/input.txt", $year, $day);

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
my $response = $ua->get($input_url, ':content_file' => $out_file);
die $response->status_line unless $response->is_success;
print "   ... success.\n";

