load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day19",
    srcs = ["day19.cc"],
    hdrs = ["day19.h"],
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
    name = "day19_test",
    size = "small",
    srcs = ["day19_test.cc"],
    data = [
        "day19.test",
    ],
    deps = [
        ":day19",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:file_test",
    ],
)
