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

cc_library(
    name = "2019",
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code/2019/day01",
        "//advent_of_code/2019/day02",
        "//advent_of_code/2019/day03",
        "//advent_of_code/2019/day04",
        "//advent_of_code/2019/day05",
        "//advent_of_code/2019/day06",
        "//advent_of_code/2019/day07",
        "//advent_of_code/2019/day08",
        "//advent_of_code/2019/day09",
        "//advent_of_code/2019/day10",
        "//advent_of_code/2019/day11",
        "//advent_of_code/2019/day12",
        "//advent_of_code/2019/day13",
        "//advent_of_code/2019/day14",
        "//advent_of_code/2019/day15",
        "//advent_of_code/2019/day16",
        "//advent_of_code/2019/day17",
        "//advent_of_code/2019/day18",
        "//advent_of_code/2019/day19",
        "//advent_of_code/2019/day20",
        "//advent_of_code/2019/day21",
        "//advent_of_code/2019/day22",
        "//advent_of_code/2019/day23",
        "//advent_of_code/2019/day24",
        "//advent_of_code/2019/day25",
    ],
)
