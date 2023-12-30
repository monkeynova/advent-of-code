load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day02",
    srcs = ["day02.cc"],
    hdrs = ["day02.h"],
    data = [
        "input.txt",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:tokenizer",
        "@com_google_absl//absl/algorithm:container",
        "@com_google_absl//absl/log",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day02_test",
    size = "small",
    srcs = ["day02_test.cc"],
    data = [
        "day02.test",
    ],
    deps = [
        ":day02",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:file_test",
    ],
)
