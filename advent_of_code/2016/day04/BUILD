load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day04",
    srcs = ["day04.cc"],
    hdrs = ["day04.h"],
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
    name = "day04_test",
    size = "small",
    srcs = ["day04_test.cc"],
    data = [
        "day04.test",
    ],
    deps = [
        ":day04",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day04_benchmark",
    srcs = ["day04_benchmark.cc"],
    size = "small",
    tags = ["benchmark"],
    data = [
        "day04.test",
    ],
    deps = [
        ":day04",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
