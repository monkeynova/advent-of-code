load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day18",
    srcs = ["day18.cc"],
    hdrs = ["day18.h"],
    data = [
        "input.txt",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:bfs",
        "//advent_of_code:point",
        "//advent_of_code:point3",
        "//advent_of_code:point_walk",
        "@com_google_absl//absl/algorithm:container",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/strings",
        "@com_googlesource_code_re2//:re2",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day18_test",
    size = "small",
    srcs = ["day18_test.cc"],
    data = [
        "day18.test",
    ],
    deps = [
        ":day18",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:file_test",
    ],
)
