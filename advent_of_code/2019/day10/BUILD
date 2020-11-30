load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day10",
    srcs = ["day10.cc"],
    hdrs = ["day10.h"],
    deps = [
        "//advent_of_code:advent_day",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
    ],
)

cc_test(
    name = "day10_test",
    srcs = ["day10_test.cc"],
    args = ["--test_file=advent_of_code/2019/day10/day10.test"],
    data = [
        "input.txt",
        "day10.test",
    ],
    deps = [
        ":day10",
        "//advent_of_code:file_test",
    ],
)
