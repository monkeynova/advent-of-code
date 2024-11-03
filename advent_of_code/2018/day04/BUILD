load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day04",
    srcs = ["day04.cc"],
    hdrs = ["day04.h"],
    data = [
        "input.txt",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code:advent_day",
        "@abseil-cpp//absl/container:flat_hash_map",
        "@abseil-cpp//absl/container:flat_hash_set",
        "@abseil-cpp//absl/log",
        "@abseil-cpp//absl/strings",
        "@re2",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day04_test",
    size = "small",
    srcs = ["day04_test.cc"],
    data = [
        "day04.test",
    ],
    deps = [
        ":day04",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:file_test",
    ],
)
