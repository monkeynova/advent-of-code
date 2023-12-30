load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day08",
    srcs = ["day08.cc"],
    hdrs = ["day08.h"],
    data = [
        "input.txt",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:char_board",
        "//advent_of_code:ocr",
        "//advent_of_code:point",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/strings",
        "@com_googlesource_code_re2//:re2",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day08_test",
    size = "small",
    srcs = ["day08_test.cc"],
    data = [
        "day08.test",
    ],
    deps = [
        ":day08",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day08_benchmark",
    size = "small",
    srcs = ["day08_benchmark.cc"],
    data = [
        "day08.test",
    ],
    tags = ["benchmark"],
    deps = [
        ":day08",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
