load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day02",
    srcs = ["day02.cc"],
    hdrs = ["day02.h"],
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
        "//advent_of_code/infra:gunit_main",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day02_benchmark",
    tags = ["benchmark"],
    deps = [
        ":day02_benchmark_lib",
        "//advent_of_code/infra:gunit_main",
    ],
)
