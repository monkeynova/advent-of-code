load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day17",
    srcs = ["day17.cc"],
    hdrs = ["day17.h"],
    data = [
        "input.txt",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:tokenizer",
        "@abseil-cpp//absl/log",
        "@abseil-cpp//absl/strings",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day17_test",
    size = "small",
    srcs = ["day17_test.cc"],
    data = [
        "day17.test",
    ],
    deps = [
        ":day17",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:file_test",
    ],
)
