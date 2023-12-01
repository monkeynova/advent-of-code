load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day09",
    srcs = ["day09.cc"],
    hdrs = ["day09.h"],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:splice_ring",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/strings",
        "@com_googlesource_code_re2//:re2",
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
    size = "small",
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
        "@com_google_googletest//:gtest",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day09_benchmark",
    size = "small",
    tags = ["benchmark"],
    deps = [
        ":day09_benchmark_lib",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
