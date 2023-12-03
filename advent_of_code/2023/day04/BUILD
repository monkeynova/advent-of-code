load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day04",
    srcs = ["day04.cc"],
    hdrs = ["day04.h"],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:bfs",
        "//advent_of_code:char_board",
        "//advent_of_code:conway",
        "//advent_of_code:directed_graph",
        "//advent_of_code:interval",
        "//advent_of_code:loop_history",
        "//advent_of_code:point",
        "//advent_of_code:point3",
        "//advent_of_code:point_walk",
        "//advent_of_code:splice_ring",
        "@com_google_absl//absl/algorithm:container",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/strings",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_library(
    name = "day04_input",
    testonly = 1,
    data = [
        "day04.test",
        "input.txt",
    ],
    deps = [
        ":day04",
    ],
)

cc_test(
    name = "day04_test",
    size = "small",
    srcs = ["day04_test.cc"],
    deps = [
        ":day04_input",
        "//advent_of_code/infra:file_test",
    ],
)

cc_library(
    name = "day04_benchmark_lib",
    testonly = 1,
    srcs = ["day04_benchmark.cc"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":day04_input",
        "//advent_of_code/infra:file_benchmark",
        "@com_google_googletest//:gtest",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day04_benchmark",
    size = "small",
    tags = ["benchmark"],
    deps = [
        ":day04_benchmark_lib",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
