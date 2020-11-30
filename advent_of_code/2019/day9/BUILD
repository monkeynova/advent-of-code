load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day9",
    srcs = ["day9.cc"],
    hdrs = ["day9.h"],
    deps = [
        "//advent_of_code:advent_day",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
    ],
)

cc_test(
    name = "day9_test",
    srcs = ["day9_test.cc"],
    args = ["--test_file=advent_of_code/2019/day9/day9.test"],
    data = [
        "input.txt",
        "day9.test",
    ],
    deps = [
        ":day9",
        "//advent_of_code:file_test",
    ],
)
