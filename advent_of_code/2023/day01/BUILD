load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day01",
    srcs = ["day01.cc"],
    hdrs = ["day01.h"],
    data = [
        "input.txt",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code:advent_day",
        "@abseil-cpp//absl/log",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day01_test",
    size = "small",
    srcs = ["day01_test.cc"],
    data = [
        "day01.test",
    ],
    deps = [
        ":day01",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:file_test",
    ],
)
