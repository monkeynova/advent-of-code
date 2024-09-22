load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day24",
    srcs = ["day24.cc"],
    hdrs = ["day24.h"],
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
    ],
    alwayslink = 1,
)

cc_test(
    name = "day24_test",
    size = "small",
    srcs = ["day24_test.cc"],
    data = [
        "day24.test",
    ],
    deps = [
        ":day24",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:file_test",
    ],
)
