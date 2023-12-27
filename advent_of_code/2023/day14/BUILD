load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day14",
    srcs = ["day14.cc"],
    hdrs = ["day14.h"],
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
        "//advent_of_code:loop_history",
        "//advent_of_code:point",
        "@com_google_absl//absl/log",
    ],
)

cc_test(
    name = "day14_test",
    size = "small",
    srcs = ["day14_test.cc"],
    data = [
        "day14.test",
    ],
    deps = [
        ":day14",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day14_benchmark",
    srcs = ["day14_benchmark.cc"],
    size = "small",
    tags = ["benchmark"],
    data = [
        "day14.test",
    ],
    deps = [
        ":day14",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
