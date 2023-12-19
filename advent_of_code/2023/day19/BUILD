load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day19",
    srcs = ["day19.cc"],
    hdrs = ["day19.h"],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:tokenizer",
        "@com_google_absl//absl/algorithm:container",
        "@com_google_absl//absl/log",
    ],
)

cc_library(
    name = "day19_input",
    testonly = 1,
    data = [
        "day19.test",
        "input.txt",
    ],
    deps = [
        ":day19",
    ],
)

cc_test(
    name = "day19_test",
    size = "small",
    srcs = ["day19_test.cc"],
    deps = [
        ":day19_input",
        "//advent_of_code/infra:file_test",
    ],
)

cc_library(
    name = "day19_benchmark_lib",
    testonly = 1,
    srcs = ["day19_benchmark.cc"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":day19_input",
        "//advent_of_code/infra:file_benchmark",
        "@com_google_googletest//:gtest",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day19_benchmark",
    size = "small",
    tags = ["benchmark"],
    deps = [
        ":day19_benchmark_lib",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
