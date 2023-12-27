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
    srcs = ["vm.cc"],
    hdrs = ["vm.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code:vlog",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_library(
    name = "2017",
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code/2017/day01",
        "//advent_of_code/2017/day02",
        "//advent_of_code/2017/day03",
        "//advent_of_code/2017/day04",
        "//advent_of_code/2017/day05",
        "//advent_of_code/2017/day06",
        "//advent_of_code/2017/day07",
        "//advent_of_code/2017/day08",
        "//advent_of_code/2017/day09",
        "//advent_of_code/2017/day10",
        "//advent_of_code/2017/day11",
        "//advent_of_code/2017/day12",
        "//advent_of_code/2017/day13",
        "//advent_of_code/2017/day14",
        "//advent_of_code/2017/day15",
        "//advent_of_code/2017/day16",
        "//advent_of_code/2017/day17",
        "//advent_of_code/2017/day18",
        "//advent_of_code/2017/day19",
        "//advent_of_code/2017/day20",
        "//advent_of_code/2017/day21",
        "//advent_of_code/2017/day22",
        "//advent_of_code/2017/day23",
        "//advent_of_code/2017/day24",
        "//advent_of_code/2017/day25",
    ],
)
