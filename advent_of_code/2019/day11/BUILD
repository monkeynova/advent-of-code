load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day11",
    srcs = ["day11.cc"],
    hdrs = ["day11.h"],
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
        "//advent_of_code/2019:int_code",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/strings",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day11_test",
    size = "small",
    srcs = ["day11_test.cc"],
    data = [
        "day11.test",
    ],
    deps = [
        ":day11",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:file_test",
    ],
)
