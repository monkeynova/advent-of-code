load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day17",
    srcs = ["day17.cc"],
    hdrs = ["day17.h"],
    data = [
        "input.txt",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:char_board",
        "//advent_of_code:loop_history",
        "//advent_of_code:point",
        "//advent_of_code:point_walk",
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
    name = "day17_test",
    size = "small",
    srcs = ["day17_test.cc"],
    data = [
        "day17.test",
    ],
    deps = [
        ":day17",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day17_benchmark",
    size = "small",
    srcs = ["day17_benchmark.cc"],
    data = [
        "day17.test",
    ],
    tags = ["benchmark"],
    deps = [
        ":day17",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
