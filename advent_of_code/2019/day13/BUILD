load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day13",
    srcs = ["day13.cc"],
    hdrs = ["day13.h"],
    data = [
        "input.txt",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:point",
        "//advent_of_code/2019:int_code",
        "@abseil-cpp//absl/container:flat_hash_map",
        "@abseil-cpp//absl/log",
        "@abseil-cpp//absl/strings",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day13_test",
    size = "small",
    srcs = ["day13_test.cc"],
    data = [
        "day13.test",
    ],
    deps = [
        ":day13",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:file_test",
    ],
)
