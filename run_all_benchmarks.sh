#!/bin/sh
 
echo > ~/benchmark_out.txt
for t in `bazel query 'filter(".*day\d+_benchmark",...)'`
do
  bazel run -c opt $t -- --benchmark | grep BM_ >> ~/benchmark_out.txt
done