load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day10",
    srcs = ["day10.cc"],
    hdrs = ["day10.h"],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:char_board",
        "//advent_of_code:point",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
    ],
)

cc_test(
    name = "day10_test",
    srcs = ["day10_test.cc"],
    args = ["--test_file=advent_of_code/2019/day10/day10.test"],
    data = [
        "day10.test",
        "input.txt",
    ],
    deps = [
        ":day10",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day10_benchmark",
    srcs = ["day10_benchmark.cc"],
    args = ["--test_file=advent_of_code/2019/day10/day10.test"],
    data = [
        "day10.test",
        "input.txt",
    ],
    deps = [
        ":day10",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
