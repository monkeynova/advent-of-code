load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "int_code",
    srcs = ["int_code.cc"],
    hdrs = ["int_code.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code:vlog",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "int_code_test",
    size = "small",
    srcs = ["int_code_test.cc"],
    args = ["--test_file=advent_of_code/2019/int_code.test"],
    data = ["int_code.test"],
    deps = [
        ":int_code",
        # Allow file flag options on all tests to avoid test failures in
        # bazelisk test ... --test_arg=<file arg>.
        "//advent_of_code/infra:file_flags",
        "@com_google_absl//absl/debugging:failure_signal_handler",
        "@com_google_file_based_test_driver//file_based_test_driver",
        "@com_google_file_based_test_driver//file_based_test_driver:run_test_case_result",
        "@com_google_file_based_test_driver//file_based_test_driver:test_case_options",
        "@com_monkeynova_gunit_main//:test_main",
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

sh_binary(
    name = "whole_year",
    testonly = 1,
    srcs = ["//tools:whole_year.sh"],
    data = [":benchmark"],
    args = ["$(location :benchmark)"],
)

cc_library(
    name = "benchmark_lib",
    testonly = 1,
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code/2019/day01:day01_benchmark_lib",
        "//advent_of_code/2019/day02:day02_benchmark_lib",
        "//advent_of_code/2019/day03:day03_benchmark_lib",
        "//advent_of_code/2019/day04:day04_benchmark_lib",
        "//advent_of_code/2019/day05:day05_benchmark_lib",
        "//advent_of_code/2019/day06:day06_benchmark_lib",
        "//advent_of_code/2019/day07:day07_benchmark_lib",
        "//advent_of_code/2019/day08:day08_benchmark_lib",
        "//advent_of_code/2019/day09:day09_benchmark_lib",
        "//advent_of_code/2019/day10:day10_benchmark_lib",
        "//advent_of_code/2019/day11:day11_benchmark_lib",
        "//advent_of_code/2019/day12:day12_benchmark_lib",
        "//advent_of_code/2019/day13:day13_benchmark_lib",
        "//advent_of_code/2019/day14:day14_benchmark_lib",
        "//advent_of_code/2019/day15:day15_benchmark_lib",
        "//advent_of_code/2019/day16:day16_benchmark_lib",
        "//advent_of_code/2019/day17:day17_benchmark_lib",
        "//advent_of_code/2019/day18:day18_benchmark_lib",
        "//advent_of_code/2019/day19:day19_benchmark_lib",
        "//advent_of_code/2019/day20:day20_benchmark_lib",
        "//advent_of_code/2019/day21:day21_benchmark_lib",
        "//advent_of_code/2019/day22:day22_benchmark_lib",
        "//advent_of_code/2019/day23:day23_benchmark_lib",
        "//advent_of_code/2019/day24:day24_benchmark_lib",
        "//advent_of_code/2019/day25:day25_benchmark_lib",
    ],
)
