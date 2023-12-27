load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day13",
    srcs = ["day13.cc"],
    hdrs = ["day13.h"],
    data = [
        "input.txt",
    ],
    visibility = [
        "//visibility:public",
    ],
    alwayslink = 1,
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:char_board",
        "@com_google_absl//absl/log",
    ],
)

cc_test(
    name = "day13_test",
    size = "small",
    srcs = ["day13_test.cc"],
    data = [
        "day13.test",
    ],
    deps = [
        ":day13",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day13_benchmark",
    srcs = ["day13_benchmark.cc"],
    size = "small",
    tags = ["benchmark"],
    data = [
        "day13.test",
    ],
    deps = [
        ":day13",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
