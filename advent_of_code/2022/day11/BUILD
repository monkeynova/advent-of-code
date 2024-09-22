load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day11",
    srcs = ["day11.cc"],
    hdrs = ["day11.h"],
    data = [
        "input.txt",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code:advent_day",
        "@abseil-cpp//absl/algorithm:container",
        "@abseil-cpp//absl/container:flat_hash_map",
        "@abseil-cpp//absl/container:flat_hash_set",
        "@abseil-cpp//absl/log",
        "@abseil-cpp//absl/strings",
        "@re2//:re2",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day11_test",
    size = "small",
    srcs = ["day11_test.cc"],
    data = [
        "day11.test",
    ],
    deps = [
        ":day11",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:file_test",
    ],
)
