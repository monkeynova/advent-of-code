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
    alwayslink = 1,
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:bfs",
        "//advent_of_code:char_board",
        "//advent_of_code:point_walk",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/strings",
        "@com_googlesource_code_re2//:re2",
    ],
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
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day15_benchmark",
    srcs = ["day15_benchmark.cc"],
    size = "small",
    tags = ["benchmark"],
    data = [
        "day15.test",
    ],
    deps = [
        ":day15",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
