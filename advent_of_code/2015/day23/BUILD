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
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/strings",
        "@com_googlesource_code_re2//:re2",
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
