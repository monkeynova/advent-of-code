load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day18",
    srcs = ["day18.cc"],
    hdrs = ["day18.h"],
    deps = [
        "//advent_of_code:advent_day",
        "@com_google_absl//absl/algorithm:container",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/strings",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_library(
    name = "day18_input",
    testonly = 1,
    data = [
        "day18.test",
        "input.txt",
    ],
    deps = [
        ":day18",
    ],
)

cc_test(
    name = "day18_test",
    size = "small",
    srcs = ["day18_test.cc"],
    deps = [
        ":day18_input",
        "//advent_of_code/infra:file_test",
    ],
)

cc_library(
    name = "day18_benchmark_lib",
    testonly = 1,
    srcs = ["day18_benchmark.cc"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":day18_input",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:gunit_main",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day18_benchmark",
    size = "small",
    tags = ["benchmark"],
    deps = [
        ":day18_benchmark_lib",
        "//advent_of_code/infra:gunit_main",
    ],
)
