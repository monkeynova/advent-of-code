load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")

cc_library(
    name = "day25",
    srcs = ["day25.cc"],
    hdrs = ["day25.h"],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code/2019:int_code",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/strings",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_binary(
    name = "day25_interactive",
    srcs = ["day25_interactive.cc"],
    data = [
        "input.txt",
    ],
    deps = [
        "//advent_of_code/2019:int_code",
        "//advent_of_code/infra:file_util",
        "@com_google_absl//absl/debugging:failure_signal_handler",
        "@com_google_absl//absl/debugging:symbolize",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/log:flags",
        "@com_google_absl//absl/log:initialize",
        "@com_google_absl//absl/strings",
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
