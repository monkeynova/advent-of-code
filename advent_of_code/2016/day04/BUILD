load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day04",
    srcs = ["day04.cc"],
    hdrs = ["day04.h"],
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
    name = "day04_input",
    testonly = 1,
    data = [
        "day04.test",
        "input.txt",
    ],
    deps = [
        ":day04",
    ],
)

cc_test(
    name = "day04_test",
    size = "small",
    srcs = ["day04_test.cc"],
    deps = [
        ":day04_input",
        "//advent_of_code/infra:file_test",
    ],
)

cc_library(
    name = "day04_benchmark_lib",
    testonly = 1,
    srcs = ["day04_benchmark.cc"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":day04_input",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:gunit_main",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day04_benchmark",
    size = "small",
    tags = ["benchmark"],
    deps = [
        ":day04_benchmark_lib",
        "//advent_of_code/infra:gunit_main",
    ],
)
