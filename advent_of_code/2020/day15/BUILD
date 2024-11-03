load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day15",
    srcs = ["day15.cc"],
    hdrs = ["day15.h"],
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
    name = "day15_test",
    size = "small",
    srcs = ["day15_test.cc"],
    data = [
        "day15.test",
    ],
    deps = [
        ":day15",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:file_test",
    ],
)
