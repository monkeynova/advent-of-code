load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day22",
    srcs = ["day22.cc"],
    hdrs = ["day22.h"],
    data = [
        "input.txt",
    ],
    visibility = [
        "//visibility:public",
    ],
    alwayslink = 1,
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:point",
        "//advent_of_code:point3",
        "//advent_of_code:tokenizer",
        "@com_google_absl//absl/algorithm:container",
        "@com_google_absl//absl/log",
    ],
)

cc_test(
    name = "day22_test",
    size = "small",
    srcs = ["day22_test.cc"],
    data = [
        "day22.test",
    ],
    deps = [
        ":day22",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day22_benchmark",
    srcs = ["day22_benchmark.cc"],
    size = "small",
    tags = ["benchmark"],
    data = [
        "day22.test",
    ],
    deps = [
        ":day22",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
