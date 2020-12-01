load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day13",
    srcs = ["day13.cc"],
    hdrs = ["day13.h"],
    deps = [
        "//advent_of_code:advent_day",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
    ],
)

cc_test(
    name = "day13_test",
    srcs = ["day13_test.cc"],
    args = ["--test_file=advent_of_code/2019/day13/day13.test"],
    data = [
        "input.txt",
        "day13.test",
    ],
    deps = [
        ":day13",
        "//advent_of_code:file_test",
    ],
)
