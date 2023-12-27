load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day24",
    srcs = ["day24.cc"],
    hdrs = ["day24.h"],
    data = [
        "input.txt",
    ],
    visibility = [
        "//visibility:public",
    ],
    alwayslink = 1,
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:interval",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/strings",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_test(
    name = "day24_test",
    size = "small",
    srcs = ["day24_test.cc"],
    data = [
        "day24.test",
    ],
    deps = [
        ":day24",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day24_benchmark",
    srcs = ["day24_benchmark.cc"],
    size = "small",
    tags = ["benchmark"],
    data = [
        "day24.test",
    ],
    deps = [
        ":day24",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
