load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day15",
    srcs = ["day15.cc"],
    hdrs = ["day15.h"],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:bfs",
        "//advent_of_code:char_board",
        "//advent_of_code:point",
        "//advent_of_code/2019:int_code",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/random",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
    ],
)

cc_test(
    name = "day15_test",
    srcs = ["day15_test.cc"],
    args = ["--test_file=advent_of_code/2019/day15/day15.test"],
    data = [
        "day15.test",
        "input.txt",
    ],
    deps = [
        ":day15",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day15_benchmark",
    srcs = ["day15_benchmark.cc"],
    args = ["--test_file=advent_of_code/2019/day15/day15.test"],
    data = [
        "day15.test",
        "input.txt",
    ],
    deps = [
        ":day15",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
