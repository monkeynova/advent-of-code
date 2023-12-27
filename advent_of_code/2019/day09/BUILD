load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day09",
    srcs = ["day09.cc"],
    hdrs = ["day09.h"],
    data = [
        "input.txt",
    ],
    visibility = [
        "//visibility:public",
    ],
    alwayslink = 1,
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code/2019:int_code",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "day09_test",
    size = "small",
    srcs = ["day09_test.cc"],
    data = [
        "day09.test",
    ],
    deps = [
        ":day09",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day09_benchmark",
    srcs = ["day09_benchmark.cc"],
    size = "small",
    tags = ["benchmark"],
    data = [
        "day09.test",
    ],
    deps = [
        ":day09",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
