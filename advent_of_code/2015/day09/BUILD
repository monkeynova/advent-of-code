load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day09",
    srcs = ["day09.cc"],
    hdrs = ["day09.h"],
    data = [
        "input.txt",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:opt_cmp",
        "@abseil-cpp//absl/container:flat_hash_map",
        "@abseil-cpp//absl/container:flat_hash_set",
        "@abseil-cpp//absl/log",
        "@abseil-cpp//absl/strings",
        "@re2",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day09_test",
    size = "small",
    srcs = ["day09_test.cc"],
    data = [
        "day09.test",
    ],
    deps = [
        ":day09",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:file_test",
    ],
)
