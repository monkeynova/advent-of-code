load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day09",
    srcs = ["day09.cc"],
    hdrs = ["day09.h"],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code/2019:int_code",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/log",
    ],
)

cc_library(
    name = "day09_input",
    testonly = 1,
    data = [
        "day09.test",
        "input.txt",
    ],
    deps = [
        ":day09",
    ],
)

cc_test(
    name = "day09_test",
    srcs = ["day09_test.cc"],
    deps = [
        ":day09_input",
        "//advent_of_code/infra:file_test",
    ],
)

cc_library(
    name = "day09_benchmark_lib",
    testonly = 1,
    srcs = ["day09_benchmark.cc"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":day09_input",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:gunit_main",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day09_benchmark",
    tags = ["benchmark"],
    deps = [
        ":day09_benchmark_lib",
        "//advent_of_code/infra:gunit_main",
    ],
)
