load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day08",
    srcs = ["day08.cc"],
    hdrs = ["day08.h"],
    deps = [
        "//advent_of_code:advent_day",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "day08_input",
    testonly = 1,
    data = [
        "day08.test",
        "input.txt",
    ],
    deps = [
        ":day08",
    ],
)

cc_test(
    name = "day08_test",
    size = "small",
    srcs = ["day08_test.cc"],
    deps = [
        ":day08_input",
        "//advent_of_code/infra:file_test",
    ],
)

cc_library(
    name = "day08_benchmark_lib",
    testonly = 1,
    srcs = ["day08_benchmark.cc"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":day08_input",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day08_benchmark",
    size = "small",
    tags = ["benchmark"],
    deps = [
        ":day08_benchmark_lib",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
