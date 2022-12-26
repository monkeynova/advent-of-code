load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day03",
    srcs = ["day03.cc"],
    hdrs = ["day03.h"],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:point",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "day03_input",
    testonly = 1,
    data = [
        "day03.test",
        "input.txt",
    ],
    deps = [
        ":day03",
    ],
)

cc_test(
    name = "day03_test",
    size = "small",
    srcs = ["day03_test.cc"],
    deps = [
        ":day03_input",
        "//advent_of_code/infra:file_test",
    ],
)

cc_library(
    name = "day03_benchmark_lib",
    testonly = 1,
    srcs = ["day03_benchmark.cc"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":day03_input",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day03_benchmark",
    size = "small",
    tags = ["benchmark"],
    deps = [
        ":day03_benchmark_lib",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
