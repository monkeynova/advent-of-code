load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day16",
    srcs = ["day16.cc"],
    hdrs = ["day16.h"],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code/2018:vm",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/strings",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_library(
    name = "day16_input",
    testonly = 1,
    data = [
        "day16.test",
        "input.txt",
    ],
    deps = [
        ":day16",
    ],
)

cc_test(
    name = "day16_test",
    size = "small",
    srcs = ["day16_test.cc"],
    deps = [
        ":day16_input",
        "//advent_of_code/infra:file_test",
    ],
)

cc_library(
    name = "day16_benchmark_lib",
    testonly = 1,
    srcs = ["day16_benchmark.cc"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":day16_input",
        "//advent_of_code/infra:file_benchmark",
        "@com_google_googletest//:gtest",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day16_benchmark",
    size = "small",
    tags = ["benchmark"],
    deps = [
        ":day16_benchmark_lib",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
