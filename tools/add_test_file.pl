#!/usr/bin/env perl

use strict;
use warnings;

my ($year, $day) = @ARGV;

die "USAGE:\n$0 <year> <day>" if !$year || !$day;

my $path = "advent_of_code/${year}/day${day}";
die "Bad path: ${path}\nUSAGE:\n$0 <year> <day>" if ! -d $path;

{
  my $fname = "${path}/day${day}.h";
  my $data = read_fully($fname);
  if ($data !~ /test_file\(\)/) {
    print "Updating ${fname} ...\n";
    my $impl = <<EOF;
 public:
  virtual absl::string_view test_file() override {
    return "advent_of_code/${year}/day${day}/day${day}.test";
  }

EOF
    $data =~ s/.*public:\n/$impl/;
    open my $fh, '>', $fname or die "cannot write to ", $fname;
    print {$fh} $data;
    close($fh);
  }
}
{
  my $fname = "${path}/day${day}_benchmark.cc";
  my $data = read_fully($fname);
  if ($data =~ /FileBenchmarkTestCount\(\)/) {
    print "Updating ${fname} ...\n";
    my $impl = <<EOF;
  Day_${year}_${day} day;
  absl::StatusOr<int> test_count = FileBenchmarkTestCount(&day);
EOF
    $data =~ s/.*FileBenchmarkTestCount\(\);\n/$impl/;
    open my $fh, '>', $fname or die "cannot write to ", $fname;
    print {$fh} $data;
    close($fh);
  }
}
{
  my $fname = "${path}/BUILD";
  my $data = read_fully($fname);
  if ($data !~ /day${day}_input/) {
    print "Updating ${fname} ...\n";
    my $impl = <<EOF;
cc_library(
    name = "day${day}_input",
    testonly = 1,
    data = [
        "day${day}.test",
        "input.txt",
    ],
    deps = [
        ":day${day}",
    ],
)

cc_test(
    name = "day${day}_test",
    srcs = ["day${day}_test.cc"],
    deps = [
        ":day${day}_input",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day${day}_benchmark",
    srcs = ["day${day}_benchmark.cc"],
    tags = ["benchmark"],
    deps = [
        ":day${day}_input",
        "//advent_of_code/infra:file_benchmark",
        "\@com_monkeynova_gunit_main//:test_main",
    ],
)
EOF
    $data =~ s/cc_test\(\s*name = "day${day}_test".*/$impl/s;
    open my $fh, '>', $fname or die "cannot write to ", $fname;
    print {$fh} $data;
    close($fh);
  }
}


sub read_fully {
  my ($fname) = @_;
  open my $fh, '<', $fname or die "cannot open ", $fname;
  my $data = join '', <$fh>;
  close($fh);
  return $data;
}