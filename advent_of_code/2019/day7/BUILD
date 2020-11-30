load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day7",
    srcs = ["day7.cc"],
    hdrs = ["day7.h"],
    deps = [
        "//advent_of_code:advent_day",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
    ],
)

cc_test(
    name = "day7_test",
    srcs = ["day7_test.cc"],
    args = ["--test_file=advent_of_code/2019/day7/day7.test"],
    data = [
        "input.txt",
        "day7.test",
    ],
    deps = [
        ":day7",
        "//advent_of_code:file_test",
    ],
)
