load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day21",
    srcs = ["day21.cc"],
    hdrs = ["day21.h"],
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
    name = "day21_input",
    testonly = 1,
    data = [
        "day21.test",
        "input.txt",
    ],
    deps = [
        ":day21",
    ],
)

cc_test(
    name = "day21_test",
    srcs = ["day21_test.cc"],
    deps = [
        ":day21_input",
        "//advent_of_code/infra:file_test",
    ],
)

cc_library(
    name = "day21_benchmark_lib",
    testonly = 1,
    srcs = ["day21_benchmark.cc"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":day21_input",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day21_benchmark",
    tags = ["benchmark"],
    deps = [
        ":day21_benchmark_lib",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
