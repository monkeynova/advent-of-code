load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day02",
    srcs = ["day02.cc"],
    hdrs = ["day02.h"],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code/2019:int_code",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
    ],
)

cc_library(
    name = "day02_input",
    testonly = 1,
    data = [
        "day02.test",
        "input.txt",
    ],
    deps = [
        ":day02",
    ],
)

cc_test(
    name = "day02_test",
    srcs = ["day02_test.cc"],
    deps = [
        ":day02_input",
        "//advent_of_code/infra:file_test",
    ],
)

cc_library(
    name = "day02_benchmark_lib",
    testonly = 1,
    srcs = ["day02_benchmark.cc"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":day02_input",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day02_benchmark",
    tags = ["benchmark"],
    deps = [
        ":day02_benchmark_lib",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
