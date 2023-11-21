load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "knot_hash",
    srcs = ["knot_hash.cc"],
    hdrs = ["knot_hash.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code:vlog",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/types:span",
    ],
)

cc_test(
    name = "knot_hash_test",
    size = "small",
    srcs = ["knot_hash_test.cc"],
    deps = [
        ":knot_hash",
        # Allow file flag options on all tests to avoid test failures in
        # bazelisk test ... --test_arg=<file arg>.
        "//advent_of_code/infra:file_flags",
        "@com_google_googletest//:gtest",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)

cc_library(
    name = "vm",
    hdrs = ["vm.h"],
    srcs = ["vm.cc"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code:vlog",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/status:statusor",
        "@com_googlesource_code_re2//:re2",
    ],
)


cc_test(
    name = "benchmark",
    size = "small",
    tags = ["benchmark"],
    deps = [
        ":benchmark_lib",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)

cc_binary(
    name = "whole_year",
    testonly = 1,
    args = [
        "--benchmark",
        "--bechmark_file_include_filter_re=input",
        "--benchmark_flags=--benchmark_min_time=0ns",
    ],
    deps = [
        ":benchmark_lib",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)

cc_library(
    name = "benchmark_lib",
    testonly = 1,
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code/2017/day01:day01_benchmark_lib",
        "//advent_of_code/2017/day02:day02_benchmark_lib",
        "//advent_of_code/2017/day03:day03_benchmark_lib",
        "//advent_of_code/2017/day04:day04_benchmark_lib",
        "//advent_of_code/2017/day05:day05_benchmark_lib",
        "//advent_of_code/2017/day06:day06_benchmark_lib",
        "//advent_of_code/2017/day07:day07_benchmark_lib",
        "//advent_of_code/2017/day08:day08_benchmark_lib",
        "//advent_of_code/2017/day09:day09_benchmark_lib",
        "//advent_of_code/2017/day10:day10_benchmark_lib",
        "//advent_of_code/2017/day11:day11_benchmark_lib",
        "//advent_of_code/2017/day12:day12_benchmark_lib",
        "//advent_of_code/2017/day13:day13_benchmark_lib",
        "//advent_of_code/2017/day14:day14_benchmark_lib",
        "//advent_of_code/2017/day15:day15_benchmark_lib",
        "//advent_of_code/2017/day16:day16_benchmark_lib",
        "//advent_of_code/2017/day17:day17_benchmark_lib",
        "//advent_of_code/2017/day18:day18_benchmark_lib",
        "//advent_of_code/2017/day19:day19_benchmark_lib",
        "//advent_of_code/2017/day20:day20_benchmark_lib",
        "//advent_of_code/2017/day21:day21_benchmark_lib",
        "//advent_of_code/2017/day22:day22_benchmark_lib",
        "//advent_of_code/2017/day23:day23_benchmark_lib",
        "//advent_of_code/2017/day24:day24_benchmark_lib",
        "//advent_of_code/2017/day25:day25_benchmark_lib",
    ],
)
