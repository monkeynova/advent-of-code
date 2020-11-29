load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day4",
    srcs = ["day4.cc"],
    hdrs = ["day4.h"],
    deps = [
        "//advent_of_code:advent_day",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
    ],
)

cc_test(
    name = "day4_test",
    srcs = ["day4_test.cc"],
    args = ["--test_file=advent_of_code/2019/day4/day4.test"],
    data = [
        "input.txt",
        "day4.test",
    ],
    deps = [
        ":day4",
        "//advent_of_code:file_test",
    ],
)
