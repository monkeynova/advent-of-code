load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day8",
    srcs = ["day8.cc"],
    hdrs = ["day8.h"],
    deps = [
        "//advent_of_code:advent_day",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
    ],
)

cc_test(
    name = "day8_test",
    srcs = ["day8_test.cc"],
    args = ["--test_file=advent_of_code/2019/day8/day8.test"],
    data = [
        "input.txt",
        "day8.test",
    ],
    deps = [
        ":day8",
        "//advent_of_code:file_test",
    ],
)
