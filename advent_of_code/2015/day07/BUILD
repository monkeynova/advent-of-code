load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day07",
    srcs = ["day07.cc"],
    hdrs = ["day07.h"],
    deps = [
        "//advent_of_code:advent_day",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_library(
    name = "day07_input",
    testonly = 1,
    data = [
        "day07.test",
        "input.txt",
    ],
    deps = [
        ":day07",
    ],
)

cc_test(
    name = "day07_test",
    srcs = ["day07_test.cc"],
    deps = [
        ":day07_input",
        "//advent_of_code/infra:file_test",
    ],
)

cc_library(
    name = "day07_benchmark_lib",
    testonly = 1,
    srcs = ["day07_benchmark.cc"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":day07_input",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day07_benchmark",
    tags = ["benchmark"],
    deps = [
        ":day07_benchmark_lib",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
