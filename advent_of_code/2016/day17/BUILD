load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day17",
    srcs = ["day17.cc"],
    hdrs = ["day17.h"],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:bfs",
        "//advent_of_code:md5",
        "//advent_of_code:point",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_test(
    name = "day17_test",
    srcs = ["day17_test.cc"],
    args = ["--test_file=advent_of_code/2016/day17/day17.test"],
    data = [
        "day17.test",
        "input.txt",
    ],
    deps = [
        ":day17",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day17_benchmark",
    srcs = ["day17_benchmark.cc"],
    args = ["--test_file=advent_of_code/2016/day17/day17.test"],
    data = [
        "day17.test",
        "input.txt",
    ],
    deps = [
        ":day17",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
