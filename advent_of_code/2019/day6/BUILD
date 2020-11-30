load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day6",
    srcs = ["day6.cc"],
    hdrs = ["day6.h"],
    deps = [
        "//advent_of_code:advent_day",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_test(
    name = "day6_test",
    srcs = ["day6_test.cc"],
    args = ["--test_file=advent_of_code/2019/day6/day6.test"],
    data = [
        "day6.test",
        "input.txt",
    ],
    deps = [
        ":day6",
        "//advent_of_code:file_test",
    ],
)
