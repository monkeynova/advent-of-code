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
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:char_board",
        "//advent_of_code:point",
        "@abseil-cpp//absl/container:flat_hash_set",
        "@abseil-cpp//absl/log",
        "@abseil-cpp//absl/strings",
    ],
    alwayslink = 1,
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
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:file_test",
    ],
)
