load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day01",
    srcs = ["day01.cc"],
    hdrs = ["day01.h"],
    data = [
        "input.txt",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:bfs",
        "//advent_of_code:char_board",
        "//advent_of_code:conway",
        "//advent_of_code:directed_graph",
        "//advent_of_code:fast_board",
        "//advent_of_code:interval",
        "//advent_of_code:loop_history",
        "//advent_of_code:mod",
        "//advent_of_code:point",
        "//advent_of_code:point3",
        "//advent_of_code:point_walk",
        "//advent_of_code:splice_ring",
        "//advent_of_code:tokenizer",
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
    name = "day01_test",
    size = "small",
    srcs = ["day01_test.cc"],
    data = [
        "day01.test",
    ],
    deps = [
        ":day01",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:file_test",
    ],
)
