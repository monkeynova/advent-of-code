#!/bin/sh
 
TMP_FILE=/tmp/benchmark_out.txt.$$

echo > $TMP_FILE
for t in `bazel query 'attr("tags", "benchmark", ...)'`
do
  bazel run -c opt $t -- --benchmark --run_long_tests=1m | grep BM_ >> $TMP_FILE
done
benchmark/merge_benchmarks.pl < $TMP_FILE > benchmark/benchmark_`hostname -s`.txt
