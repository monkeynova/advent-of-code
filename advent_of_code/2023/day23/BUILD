load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day23",
    srcs = ["day23.cc"],
    hdrs = ["day23.h"],
    data = [
        "input.txt",
    ],
    visibility = [
        "//visibility:public",
    ],
    alwayslink = 1,
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:char_board",
        "//advent_of_code:fast_board",
        "//advent_of_code:point",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/log",
    ],
)

cc_test(
    name = "day23_test",
    size = "small",
    srcs = ["day23_test.cc"],
    data = [
        "day23.test",
    ],
    deps = [
        ":day23",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day23_benchmark",
    srcs = ["day23_benchmark.cc"],
    size = "small",
    tags = ["benchmark"],
    data = [
        "day23.test",
    ],
    deps = [
        ":day23",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
