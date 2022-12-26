load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day14",
    srcs = ["day14.cc"],
    hdrs = ["day14.h"],
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
    name = "day14_input",
    testonly = 1,
    data = [
        "day14.test",
        "input.txt",
    ],
    deps = [
        ":day14",
    ],
)

cc_test(
    name = "day14_test",
    size = "small",
    srcs = ["day14_test.cc"],
    deps = [
        ":day14_input",
        "//advent_of_code/infra:file_test",
    ],
)

cc_library(
    name = "day14_benchmark_lib",
    testonly = 1,
    srcs = ["day14_benchmark.cc"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":day14_input",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day14_benchmark",
    size = "small",
    tags = ["benchmark"],
    deps = [
        ":day14_benchmark_lib",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
