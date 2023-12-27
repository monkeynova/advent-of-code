load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day07",
    srcs = ["day07.cc"],
    hdrs = ["day07.h"],
    data = [
        "input.txt",
    ],
    visibility = [
        "//visibility:public",
    ],
    alwayslink = 1,
    deps = [
        "//advent_of_code:advent_day",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/strings",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_test(
    name = "day07_test",
    size = "small",
    srcs = ["day07_test.cc"],
    data = [
        "day07.test",
    ],
    deps = [
        ":day07",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day07_benchmark",
    srcs = ["day07_benchmark.cc"],
    size = "small",
    tags = ["benchmark"],
    data = [
        "day07.test",
    ],
    deps = [
        ":day07",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
