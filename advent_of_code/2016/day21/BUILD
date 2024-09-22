load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day21",
    srcs = ["day21.cc"],
    hdrs = ["day21.h"],
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
        "@re2//:re2",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day21_test",
    size = "small",
    srcs = ["day21_test.cc"],
    data = [
        "day21.test",
    ],
    deps = [
        ":day21",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:file_test",
    ],
)
