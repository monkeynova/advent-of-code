load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day3",
    srcs = ["day3.cc"],
    hdrs = ["day3.h"],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:point",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
    ],
)

cc_test(
    name = "day3_test",
    srcs = ["day3_test.cc"],
    args = ["--test_file=advent_of_code/2019/day3/day3.test"],
    data = [
        "day3.test",
        "input.txt",
    ],
    deps = [
        ":day3",
        "//advent_of_code:file_test",
    ],
)
