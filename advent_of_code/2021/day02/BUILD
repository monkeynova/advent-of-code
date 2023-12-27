load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day02",
    srcs = ["day02.cc"],
    hdrs = ["day02.h"],
    data = [
        "input.txt",
    ],
    visibility = [
        "//visibility:public",
    ],
    alwayslink = 1,
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:point",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/strings",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_test(
    name = "day02_test",
    size = "small",
    srcs = ["day02_test.cc"],
    data = [
        "day02.test",
    ],
    deps = [
        ":day02",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day02_benchmark",
    srcs = ["day02_benchmark.cc"],
    size = "small",
    tags = ["benchmark"],
    data = [
        "day02.test",
    ],
    deps = [
        ":day02",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
