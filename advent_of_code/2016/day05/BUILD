load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day05",
    srcs = ["day05.cc"],
    hdrs = ["day05.h"],
    data = [
        "input.txt",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:md5",
        "@abseil-cpp//absl/container:flat_hash_map",
        "@abseil-cpp//absl/container:flat_hash_set",
        "@abseil-cpp//absl/log",
        "@abseil-cpp//absl/strings",
        "@re2",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day05_test",
    size = "small",
    srcs = ["day05_test.cc"],
    data = [
        "day05.test",
    ],
    deps = [
        ":day05",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:file_test",
    ],
)
