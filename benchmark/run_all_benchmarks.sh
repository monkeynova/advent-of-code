#!/bin/sh

for YEAR in 20{15,16,17,18,19,20,21}
do
  benchmark/run_year_benchmark.pl $YEAR
done