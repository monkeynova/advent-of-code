load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day10",
    srcs = ["day10.cc"],
    hdrs = ["day10.h"],
    data = [
        "input.txt",
    ],
    visibility = [
        "//visibility:public",
    ],
    alwayslink = 1,
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:char_board",
        "//advent_of_code:point",
        "@com_google_absl//absl/algorithm:container",
        "@com_google_absl//absl/log",
    ],
)

cc_test(
    name = "day10_test",
    size = "small",
    srcs = ["day10_test.cc"],
    data = [
        "day10.test",
    ],
    deps = [
        ":day10",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day10_benchmark",
    srcs = ["day10_benchmark.cc"],
    size = "small",
    tags = ["benchmark"],
    data = [
        "day10.test",
    ],
    deps = [
        ":day10",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
