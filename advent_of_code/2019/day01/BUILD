load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day01",
    srcs = ["day01.cc"],
    hdrs = ["day01.h"],
    deps = [
        "//advent_of_code:advent_day",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "day01_input",
    testonly = 1,
    data = [
        "day01.test",
        "input.txt",
    ],
    deps = [
        ":day01",
    ],
)

cc_test(
    name = "day01_test",
    size = "small",
    srcs = ["day01_test.cc"],
    deps = [
        ":day01_input",
        "//advent_of_code/infra:file_test",
    ],
)

cc_library(
    name = "day01_benchmark_lib",
    testonly = 1,
    srcs = ["day01_benchmark.cc"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":day01_input",
        "//advent_of_code/infra:file_benchmark",
        "@com_google_googletest//:gtest",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day01_benchmark",
    size = "small",
    tags = ["benchmark"],
    deps = [
        ":day01_benchmark_lib",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
