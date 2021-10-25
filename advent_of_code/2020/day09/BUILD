load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day09",
    srcs = ["day09.cc"],
    hdrs = ["day09.h"],
    deps = [
        "//advent_of_code:advent_day",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_test(
    name = "day09_test",
    srcs = ["day09_test.cc"],
    args = ["--test_file=advent_of_code/2020/day09/day09.test"],
    data = [
        "day09.test",
        "input.txt",
    ],
    deps = [
        ":day09",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day09_benchmark",
    srcs = ["day09_benchmark.cc"],
    args = ["--test_file=advent_of_code/2020/day09/day09.test"],
    data = [
        "day09.test",
        "input.txt",
    ],
    tags = ["benchmark"],
    deps = [
        ":day09",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
