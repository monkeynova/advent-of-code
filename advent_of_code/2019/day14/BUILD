load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day14",
    srcs = ["day14.cc"],
    hdrs = ["day14.h"],
    deps = [
        "//advent_of_code:advent_day",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
    ],
)

cc_test(
    name = "day14_test",
    srcs = ["day14_test.cc"],
    args = ["--test_file=advent_of_code/2019/day14/day14.test"],
    data = [
        "day14.test",
        "input.txt",
    ],
    deps = [
        ":day14",
        "//advent_of_code:file_test",
    ],
)
