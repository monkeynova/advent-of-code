load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day23",
    srcs = ["day23.cc"],
    hdrs = ["day23.h"],
    data = [
        "input.txt",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:directed_graph",
        "@abseil-cpp//absl/algorithm:container",
        "@abseil-cpp//absl/container:flat_hash_set",
        "@abseil-cpp//absl/log",
        "@abseil-cpp//absl/strings",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day23_test",
    size = "small",
    srcs = ["day23_test.cc"],
    data = [
        "day23.test",
    ],
    deps = [
        ":day23",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:file_test",
    ],
)
