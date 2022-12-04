load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day01",
    srcs = ["day01.cc"],
    hdrs = ["day01.h"],
    deps = [
        "//advent_of_code:advent_day",
        "@com_google_absl//absl/algorithm:container",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/log",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_library(
    name = "day01_input",
    testonly = 1,
    data = [
        "day01.test",
        "input.txt",
    ],
    deps = [
        ":day01",
    ],
)

cc_test(
    name = "day01_test",
    srcs = ["day01_test.cc"],
    deps = [
        ":day01_input",
        "//advent_of_code/infra:file_test",
    ],
)

cc_library(
    name = "day01_benchmark_lib",
    testonly = 1,
    srcs = ["day01_benchmark.cc"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":day01_input",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:gunit_main",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day01_benchmark",
    tags = ["benchmark"],
    deps = [
        ":day01_benchmark_lib",
        "//advent_of_code/infra:gunit_main",
    ],
)
