load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day06",
    srcs = ["day06.cc"],
    hdrs = ["day06.h"],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:tokenizer",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "day06_input",
    testonly = 1,
    data = [
        "day06.test",
        "input.txt",
    ],
    deps = [
        ":day06",
    ],
)

cc_test(
    name = "day06_test",
    size = "small",
    srcs = ["day06_test.cc"],
    deps = [
        ":day06_input",
        "//advent_of_code/infra:file_test",
    ],
)

cc_library(
    name = "day06_benchmark_lib",
    testonly = 1,
    srcs = ["day06_benchmark.cc"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":day06_input",
        "//advent_of_code/infra:file_benchmark",
        "@com_google_googletest//:gtest",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day06_benchmark",
    size = "small",
    tags = ["benchmark"],
    deps = [
        ":day06_benchmark_lib",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
