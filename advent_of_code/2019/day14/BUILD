load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day14",
    srcs = ["day14.cc"],
    hdrs = ["day14.h"],
    data = [
        "input.txt",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:directed_graph",
        "//advent_of_code:interval",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/strings",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day14_test",
    size = "small",
    srcs = ["day14_test.cc"],
    data = [
        "day14.test",
    ],
    deps = [
        ":day14",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:file_test",
    ],
)
