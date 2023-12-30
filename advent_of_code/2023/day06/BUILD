load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day06",
    srcs = ["day06.cc"],
    hdrs = ["day06.h"],
    data = [
        "input.txt",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:tokenizer",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/strings",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day06_test",
    size = "small",
    srcs = ["day06_test.cc"],
    data = [
        "day06.test",
    ],
    deps = [
        ":day06",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day06_benchmark",
    size = "small",
    srcs = ["day06_benchmark.cc"],
    data = [
        "day06.test",
    ],
    tags = ["benchmark"],
    deps = [
        ":day06",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
