load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day16",
    srcs = ["day16.cc"],
    hdrs = ["day16.h"],
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
        "//advent_of_code:fast_board",
        "//advent_of_code:point",
        "@com_google_absl//absl/log",
    ],
)

cc_test(
    name = "day16_test",
    size = "small",
    srcs = ["day16_test.cc"],
    data = [
        "day16.test",
    ],
    deps = [
        ":day16",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day16_benchmark",
    srcs = ["day16_benchmark.cc"],
    size = "small",
    tags = ["benchmark"],
    data = [
        "day16.test",
    ],
    deps = [
        ":day16",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
