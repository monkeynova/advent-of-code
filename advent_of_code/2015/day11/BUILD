load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day11",
    srcs = ["day11.cc"],
    hdrs = ["day11.h"],
    deps = [
        "//advent_of_code:advent_day",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/log",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_library(
    name = "day11_input",
    testonly = 1,
    data = [
        "day11.test",
        "input.txt",
    ],
    deps = [
        ":day11",
    ],
)

cc_test(
    name = "day11_test",
    srcs = ["day11_test.cc"],
    deps = [
        ":day11_input",
        "//advent_of_code/infra:file_test",
    ],
)

cc_library(
    name = "day11_benchmark_lib",
    testonly = 1,
    srcs = ["day11_benchmark.cc"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":day11_input",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:gunit_main",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day11_benchmark",
    tags = ["benchmark"],
    deps = [
        ":day11_benchmark_lib",
        "//advent_of_code/infra:gunit_main",
    ],
)
