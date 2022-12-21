load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day05",
    srcs = ["day05.cc"],
    hdrs = ["day05.h"],
    deps = [
        "//advent_of_code:advent_day",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/strings",
        "@com_googlesource_code_re2//:re2",
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
    size = "small",
    srcs = ["day05_test.cc"],
    deps = [
        ":day05_input",
        "//advent_of_code/infra:file_test",
    ],
)

cc_library(
    name = "day05_benchmark_lib",
    testonly = 1,
    srcs = ["day05_benchmark.cc"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":day05_input",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:gunit_main",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day05_benchmark",
    size = "small",
    tags = ["benchmark"],
    deps = [
        ":day05_benchmark_lib",
        "//advent_of_code/infra:gunit_main",
    ],
)
