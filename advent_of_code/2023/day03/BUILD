load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day03",
    srcs = ["day03.cc"],
    hdrs = ["day03.h"],
    data = [
        "input.txt",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:char_board",
        "//advent_of_code:point",
        "@com_google_absl//absl/log",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day03_test",
    size = "small",
    srcs = ["day03_test.cc"],
    data = [
        "day03.test",
    ],
    deps = [
        ":day03",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:file_test",
    ],
)
