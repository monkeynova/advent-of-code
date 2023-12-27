load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day17",
    srcs = ["day17.cc"],
    hdrs = ["day17.h"],
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
        "@com_google_absl//absl/log",
    ],
)

cc_test(
    name = "day17_test",
    size = "small",
    srcs = ["day17_test.cc"],
    data = [
        "day17.test",
    ],
    deps = [
        ":day17",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day17_benchmark",
    srcs = ["day17_benchmark.cc"],
    size = "small",
    tags = ["benchmark"],
    data = [
        "day17.test",
    ],
    deps = [
        ":day17",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
