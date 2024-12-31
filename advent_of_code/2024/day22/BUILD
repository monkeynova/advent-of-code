load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day22",
    srcs = ["day22.cc"],
    hdrs = ["day22.h"],
    data = [
        "input.txt",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code:advent_day",
        "@abseil-cpp//absl/algorithm:container",
        "@abseil-cpp//absl/log",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day22_test",
    size = "small",
    srcs = ["day22_test.cc"],
    data = [
        "day22.test",
    ],
    deps = [
        ":day22",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:file_test",
    ],
)
