load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "assem_bunny",
    srcs = ["assem_bunny.cc"],
    hdrs = ["assem_bunny.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code:advent_day",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_test(
    name = "benchmark",
    size = "small",
    tags = ["benchmark"],
    deps = [
        "//advent_of_code/2016/day01:day01_benchmark_lib",
        "//advent_of_code/2016/day02:day02_benchmark_lib",
        "//advent_of_code/2016/day03:day03_benchmark_lib",
        "//advent_of_code/2016/day04:day04_benchmark_lib",
        "//advent_of_code/2016/day05:day05_benchmark_lib",
        "//advent_of_code/2016/day06:day06_benchmark_lib",
        "//advent_of_code/2016/day07:day07_benchmark_lib",
        "//advent_of_code/2016/day08:day08_benchmark_lib",
        "//advent_of_code/2016/day09:day09_benchmark_lib",
        "//advent_of_code/2016/day10:day10_benchmark_lib",
        "//advent_of_code/2016/day11:day11_benchmark_lib",
        "//advent_of_code/2016/day12:day12_benchmark_lib",
        "//advent_of_code/2016/day13:day13_benchmark_lib",
        "//advent_of_code/2016/day14:day14_benchmark_lib",
        "//advent_of_code/2016/day15:day15_benchmark_lib",
        "//advent_of_code/2016/day16:day16_benchmark_lib",
        "//advent_of_code/2016/day17:day17_benchmark_lib",
        "//advent_of_code/2016/day18:day18_benchmark_lib",
        "//advent_of_code/2016/day19:day19_benchmark_lib",
        "//advent_of_code/2016/day20:day20_benchmark_lib",
        "//advent_of_code/2016/day21:day21_benchmark_lib",
        "//advent_of_code/2016/day22:day22_benchmark_lib",
        "//advent_of_code/2016/day23:day23_benchmark_lib",
        "//advent_of_code/2016/day24:day24_benchmark_lib",
        "//advent_of_code/2016/day25:day25_benchmark_lib",
        "//advent_of_code/infra:gunit_main",
    ],
)
