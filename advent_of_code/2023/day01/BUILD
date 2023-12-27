load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day01",
    srcs = ["day01.cc"],
    hdrs = ["day01.h"],
    data = [
        "input.txt",
    ],
    visibility = [
        "//visibility:public",
    ],
    alwayslink = 1,
    deps = [
        "//advent_of_code:advent_day",
        "@com_google_absl//absl/log",
    ],
)

cc_test(
    name = "day01_test",
    size = "small",
    srcs = ["day01_test.cc"],
    data = [
        "day01.test",
    ],
    deps = [
        ":day01",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day01_benchmark",
    srcs = ["day01_benchmark.cc"],
    size = "small",
    tags = ["benchmark"],
    data = [
        "day01.test",
    ],
    deps = [
        ":day01",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
