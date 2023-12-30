load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day07",
    srcs = ["day07.cc"],
    hdrs = ["day07.h"],
    data = [
        "input.txt",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code:advent_day",
        "@com_google_absl//absl/algorithm:container",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/strings",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day07_test",
    size = "small",
    srcs = ["day07_test.cc"],
    data = [
        "day07.test",
    ],
    deps = [
        ":day07",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:file_test",
    ],
)
