load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day21",
    srcs = ["day21.cc"],
    hdrs = ["day21.h"],
    data = [
        "input.txt",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code/2018:vm",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/strings",
        "@com_googlesource_code_re2//:re2",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day21_test",
    size = "small",
    srcs = ["day21_test.cc"],
    data = [
        "day21.test",
    ],
    deps = [
        ":day21",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day21_benchmark",
    size = "small",
    srcs = ["day21_benchmark.cc"],
    data = [
        "day21.test",
    ],
    tags = ["benchmark"],
    deps = [
        ":day21",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
