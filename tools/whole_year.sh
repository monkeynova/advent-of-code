#!/bin/sh

time "$1" \
    --benchmark \
    --bechmark_file_include_filter=input \
    --benchmark_flags=--benchmark_min_time=0ns
