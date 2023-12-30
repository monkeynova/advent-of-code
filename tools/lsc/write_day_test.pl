#!/usr/bin/env perl

use strict;
use warnings;

my ($year, $day) = @ARGV;
die unless $day;
die unless -d "advent_of_code/${year}/day${day}";
die unless -f "advent_of_code/${year}/day${day}/day${day}_test.cc";

open my $ofh, '>', "advent_of_code/${year}/day${day}/day${day}_test.cc" or die $!;
print {$ofh} <<EOF;
#include "advent_of_code/${year}/day${day}/day${day}.h"

#include "advent_of_code/infra/file_benchmark.h"
#include "advent_of_code/infra/file_test.h"
#include "gmock/gmock.h"

namespace advent_of_code {

TEST_F(FileTest_Day, FileBasedTest) { EXPECT_TRUE(RunTest<Day_${year}_${day}>()); }

BENCHMARK_TEMPLATE(BM_Day, Day_${year}_${day})
    ->DenseRange(0, FileBenchmarkMaxIdx<Day_${year}_${day}>());

}  // namespace advent_of_code
EOF
close $ofh or die $!;