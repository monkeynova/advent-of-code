load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day09",
    srcs = ["day09.cc"],
    hdrs = ["day09.h"],
    data = [
        "input.txt",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:point",
        "@com_google_absl//absl/algorithm:container",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/strings",
        "@com_googlesource_code_re2//:re2",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day09_test",
    size = "small",
    srcs = ["day09_test.cc"],
    data = [
        "day09.test",
    ],
    deps = [
        ":day09",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day09_benchmark",
    size = "small",
    srcs = ["day09_benchmark.cc"],
    data = [
        "day09.test",
    ],
    tags = ["benchmark"],
    deps = [
        ":day09",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
