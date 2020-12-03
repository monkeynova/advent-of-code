load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day11",
    srcs = ["day11.cc"],
    hdrs = ["day11.h"],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:point",
        "//advent_of_code/2019:int_code",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
    ],
)

cc_test(
    name = "day11_test",
    srcs = ["day11_test.cc"],
    args = ["--test_file=advent_of_code/2019/day11/day11.test"],
    data = [
        "day11.test",
        "input.txt",
    ],
    deps = [
        ":day11",
        "//advent_of_code:file_test",
    ],
)
