load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day10",
    srcs = ["day10.cc"],
    hdrs = ["day10.h"],
    deps = [
        "//advent_of_code:advent_day",
        "@com_google_absl//absl/algorithm:container",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_library(
    name = "day10_input",
    testonly = 1,
    data = [
        "day10.test",
        "input.txt",
    ],
    deps = [
        ":day10",
    ],
)

cc_test(
    name = "day10_test",
    srcs = ["day10_test.cc"],
    deps = [
        ":day10_input",
        "//advent_of_code/infra:file_test",
    ],
)

cc_library(
    name = "day10_benchmark_lib",
    srcs = ["day10_benchmark.cc"],
    testonly = 1,
    alwayslink = 1,
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":day10_input",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)

cc_test(
    name = "day10_benchmark",
    tags = ["benchmark"],
    deps = [
        ":day10_benchmark_lib",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)

