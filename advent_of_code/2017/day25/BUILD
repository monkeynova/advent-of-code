load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day25",
    srcs = ["day25.cc"],
    hdrs = ["day25.h"],
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
    name = "day25_input",
    testonly = 1,
    data = [
        "day25.test",
        "input.txt",
    ],
    deps = [
        ":day25",
    ],
)

cc_test(
    name = "day25_test",
    srcs = ["day25_test.cc"],
    deps = [
        ":day25_input",
        "//advent_of_code/infra:file_test",
    ],
)

cc_library(
    name = "day25_benchmark_lib",
    testonly = 1,
    srcs = ["day25_benchmark.cc"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":day25_input",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:gunit_main",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day25_benchmark",
    tags = ["benchmark"],
    deps = [
        ":day25_benchmark_lib",
        "//advent_of_code/infra:gunit_main",
    ],
)
