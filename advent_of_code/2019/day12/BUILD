load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day12",
    srcs = ["day12.cc"],
    hdrs = ["day12.h"],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:point",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/strings",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_library(
    name = "day12_input",
    testonly = 1,
    data = [
        "day12.test",
        "input.txt",
    ],
    deps = [
        ":day12",
    ],
)

cc_test(
    name = "day12_test",
    srcs = ["day12_test.cc"],
    deps = [
        ":day12_input",
        "//advent_of_code/infra:file_test",
    ],
)

cc_library(
    name = "day12_benchmark_lib",
    testonly = 1,
    srcs = ["day12_benchmark.cc"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":day12_input",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:gunit_main",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day12_benchmark",
    tags = ["benchmark"],
    deps = [
        ":day12_benchmark_lib",
        "//advent_of_code/infra:gunit_main",
    ],
)
