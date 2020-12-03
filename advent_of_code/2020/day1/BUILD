load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day1",
    srcs = ["day1.cc"],
    hdrs = ["day1.h"],
    deps = [
        "//advent_of_code:advent_day",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
    ],
)

cc_test(
    name = "day1_test",
    srcs = ["day1_test.cc"],
    args = ["--test_file=advent_of_code/2020/day1/day1.test"],
    data = [
        "day1.test",
        "input.txt",
    ],
    deps = [
        ":day1",
        "//advent_of_code:file_test",
    ],
)

cc_test(
    name = "day1_benchmark",
    srcs = ["day1_benchmark.cc"],
    args = ["--test_file=advent_of_code/2020/day1/day1.test"],
    data = [
        "day1.test",
        "input.txt",
    ],
    deps = [
        ":day1",
        "//advent_of_code:file_util",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/strings",
        "@com_google_benchmark//:benchmark",
        "@com_google_glog//:glog",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
