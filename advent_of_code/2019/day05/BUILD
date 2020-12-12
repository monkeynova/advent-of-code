load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day05",
    srcs = ["day05.cc"],
    hdrs = ["day05.h"],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code/2019:int_code",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
    ],
)

cc_test(
    name = "day05_test",
    srcs = ["day05_test.cc"],
    args = ["--test_file=advent_of_code/2019/day05/day05.test"],
    data = [
        "day05.test",
        "input.txt",
    ],
    deps = [
        ":day05",
        "//advent_of_code:file_test",
    ],
)

cc_test(
    name = "day05_benchmark",
    srcs = ["day05_benchmark.cc"],
    args = ["--test_file=advent_of_code/2019/day05/day05.test"],
    data = [
        "day05.test",
        "input.txt",
    ],
    deps = [
        ":day05",
        "//advent_of_code:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
