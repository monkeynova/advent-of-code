#!/bin/sh
 
TMP_FILE=/tmp/benchmark_out.txt.$$

echo > $TMP_FILE
for t in `bazel query 'filter(".*day\d+_benchmark",...)'`
do
  bazel run -c opt $t -- --benchmark --run_long_tests=20s | grep BM_ >> $TMP_FILE
done
benchmark/sort_benchmark.pl < $TMP_FILE > benchmark/benchmark_out.txt