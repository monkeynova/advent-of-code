load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day19",
    srcs = ["day19.cc"],
    hdrs = ["day19.h"],
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
    name = "day19_test",
    size = "small",
    srcs = ["day19_test.cc"],
    data = [
        "day19.test",
    ],
    deps = [
        ":day19",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day19_benchmark",
    srcs = ["day19_benchmark.cc"],
    size = "small",
    tags = ["benchmark"],
    data = [
        "day19.test",
    ],
    deps = [
        ":day19",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
