load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day24",
    srcs = ["day24.cc"],
    hdrs = ["day24.h"],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:bfs",
        "//advent_of_code:char_board",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_test(
    name = "day24_test",
    srcs = ["day24_test.cc"],
    args = ["--test_file=advent_of_code/2016/day24/day24.test"],
    data = [
        "day24.test",
        "input.txt",
    ],
    deps = [
        ":day24",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day24_benchmark",
    srcs = ["day24_benchmark.cc"],
    args = ["--test_file=advent_of_code/2016/day24/day24.test"],
    data = [
        "day24.test",
        "input.txt",
    ],
    tags = ["benchmark"],
    deps = [
        ":day24",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
