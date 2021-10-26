load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day20",
    srcs = ["day20.cc"],
    hdrs = ["day20.h"],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:bfs",
        "//advent_of_code:char_board",
        "//advent_of_code:point",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_test(
    name = "day20_test",
    srcs = ["day20_test.cc"],
    args = ["--test_file=advent_of_code/2019/day20/day20.test"],
    data = [
        "day20.test",
        "input.txt",
    ],
    deps = [
        ":day20",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day20_benchmark",
    srcs = ["day20_benchmark.cc"],
    args = ["--test_file=advent_of_code/2019/day20/day20.test"],
    data = [
        "day20.test",
        "input.txt",
    ],
    tags = ["benchmark"],
    deps = [
        ":day20",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
