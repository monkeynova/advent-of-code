load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day15",
    srcs = ["day15.cc"],
    hdrs = ["day15.h"],
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
    name = "day15_input",
    testonly = 1,
    data = [
        "day15.test",
        "input.txt",
    ],
    deps = [
        ":day15",
    ],
)

cc_test(
    name = "day15_test",
    srcs = ["day15_test.cc"],
    deps = [
        ":day15_input",
        "//advent_of_code/infra:file_test",
    ],
)

cc_library(
    name = "day15_benchmark_lib",
    srcs = ["day15_benchmark.cc"],
    testonly = 1,
    alwayslink = 1,
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":day15_input",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)

cc_test(
    name = "day15_benchmark",
    tags = ["benchmark"],
    deps = [
        ":day15_benchmark_lib",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)

