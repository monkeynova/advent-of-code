load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day12",
    srcs = ["day12.cc"],
    hdrs = ["day12.h"],
    deps = [
        "//advent_of_code:advent_day",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_test(
    name = "day12_test",
    srcs = ["day12_test.cc"],
    args = ["--test_file=advent_of_code/2019/day12/day12.test"],
    data = [
        "day12.test",
        "input.txt",
    ],
    deps = [
        ":day12",
        "//advent_of_code:file_test",
    ],
)
