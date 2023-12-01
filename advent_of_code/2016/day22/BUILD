load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day22",
    srcs = ["day22.cc"],
    hdrs = ["day22.h"],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:char_board",
        "//advent_of_code:point",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/strings",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_library(
    name = "day22_input",
    testonly = 1,
    data = [
        "day22.test",
        "input.txt",
    ],
    deps = [
        ":day22",
    ],
)

cc_test(
    name = "day22_test",
    size = "small",
    srcs = ["day22_test.cc"],
    deps = [
        ":day22_input",
        "//advent_of_code/infra:file_test",
    ],
)

cc_library(
    name = "day22_benchmark_lib",
    testonly = 1,
    srcs = ["day22_benchmark.cc"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":day22_input",
        "//advent_of_code/infra:file_benchmark",
        "@com_google_googletest//:gtest",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day22_benchmark",
    size = "small",
    tags = ["benchmark"],
    deps = [
        ":day22_benchmark_lib",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
