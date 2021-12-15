load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day05",
    srcs = ["day05.cc"],
    hdrs = ["day05.h"],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code/2019:int_code",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
    ],
)

cc_library(
    name = "day05_input",
    testonly = 1,
    data = [
        "day05.test",
        "input.txt",
    ],
    deps = [
        ":day05",
    ],
)

cc_test(
    name = "day05_test",
    srcs = ["day05_test.cc"],
    deps = [
        ":day05_input",
        "//advent_of_code/infra:file_test",
    ],
)

cc_library(
    name = "day05_benchmark_lib",
    srcs = ["day05_benchmark.cc"],
    testonly = 1,
    alwayslink = 1,
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":day05_input",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)

cc_test(
    name = "day05_benchmark",
    tags = ["benchmark"],
    deps = [
        ":day05_benchmark_lib",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)

