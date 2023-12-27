load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day12",
    srcs = ["day12.cc"],
    hdrs = ["day12.h"],
    data = [
        "input.txt",
    ],
    visibility = [
        "//visibility:public",
    ],
    alwayslink = 1,
    deps = [
        "//advent_of_code:advent_day",
        "@com_google_absl//absl/algorithm:container",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "day12_test",
    size = "small",
    srcs = ["day12_test.cc"],
    data = [
        "day12.test",
    ],
    deps = [
        ":day12",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day12_benchmark",
    srcs = ["day12_benchmark.cc"],
    size = "small",
    tags = ["benchmark"],
    data = [
        "day12.test",
    ],
    deps = [
        ":day12",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
