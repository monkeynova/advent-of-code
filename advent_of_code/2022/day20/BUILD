load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day20",
    srcs = ["day20.cc"],
    hdrs = ["day20.h"],
    data = [
        "input.txt",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:splice_ring",
        "@abseil-cpp//absl/algorithm:container",
        "@abseil-cpp//absl/container:flat_hash_map",
        "@abseil-cpp//absl/container:flat_hash_set",
        "@abseil-cpp//absl/log",
        "@abseil-cpp//absl/strings",
        "@re2",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day20_test",
    size = "small",
    srcs = ["day20_test.cc"],
    data = [
        "day20.test",
    ],
    deps = [
        ":day20",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:file_test",
    ],
)
