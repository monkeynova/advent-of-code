load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day18",
    srcs = ["day18.cc"],
    hdrs = ["day18.h"],
    data = [
        "input.txt",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:char_board",
        "//advent_of_code:conway",
        "//advent_of_code:loop_history",
        "@abseil-cpp//absl/log",
        "@abseil-cpp//absl/strings",
        "@re2",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day18_test",
    size = "small",
    srcs = ["day18_test.cc"],
    data = [
        "day18.test",
    ],
    deps = [
        ":day18",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:file_test",
    ],
)
