#!/bin/sh

time "$1" \
    --benchmark \
    --bechmark_file_include_filter_re=input \
    --benchmark_flags=--benchmark_min_time=0ns
