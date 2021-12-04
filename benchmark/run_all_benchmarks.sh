#!/bin/sh
 
TMP_FILE=/tmp/benchmark_out.txt.$$
# SUBPATH="advent_of_code/2021"
if [ -z "$SUBPATH" ]
then
  SUBPATH="."
  BAZEL_TARGETS="..."
else
  BAZEL_TARGETS="${SUBPATH}/..."
fi

echo > $TMP_FILE
for t in `bazelisk query 'attr("tags", "benchmark", '${BAZEL_TARGETS}')'`
do
  bazelisk run -c opt $t -- --benchmark --run_long_tests=1m | grep BM_ >> $TMP_FILE
done
benchmark/merge_benchmarks.pl < $TMP_FILE > ${SUBPATH}/benchmark/benchmark_`hostname -s`.txt
