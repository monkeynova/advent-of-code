load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day16",
    srcs = ["day16.cc"],
    hdrs = ["day16.h"],
    data = [
        "input.txt",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code/2018:vm",
        "@abseil-cpp//absl/container:flat_hash_map",
        "@abseil-cpp//absl/container:flat_hash_set",
        "@abseil-cpp//absl/log",
        "@abseil-cpp//absl/strings",
        "@re2//:re2",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day16_test",
    size = "small",
    srcs = ["day16_test.cc"],
    data = [
        "day16.test",
    ],
    deps = [
        ":day16",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:file_test",
    ],
)
