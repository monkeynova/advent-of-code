load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day17",
    srcs = ["day17.cc"],
    hdrs = ["day17.h"],
    data = [
        "input.txt",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:bfs",
        "//advent_of_code:md5",
        "//advent_of_code:point",
        "@abseil-cpp//absl/container:flat_hash_map",
        "@abseil-cpp//absl/container:flat_hash_set",
        "@abseil-cpp//absl/log",
        "@abseil-cpp//absl/strings",
        "@re2",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day17_test",
    size = "small",
    srcs = ["day17_test.cc"],
    data = [
        "day17.test",
    ],
    deps = [
        ":day17",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:file_test",
    ],
)
