load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day14",
    srcs = ["day14.cc"],
    hdrs = ["day14.h"],
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
    name = "day14_test",
    srcs = ["day14_test.cc"],
    args = ["--test_file=advent_of_code/2018/day14/day14.test"],
    data = [
        "day14.test",
        "input.txt",
    ],
    deps = [
        ":day14",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day14_benchmark",
    srcs = ["day14_benchmark.cc"],
    args = ["--test_file=advent_of_code/2018/day14/day14.test"],
    data = [
        "day14.test",
        "input.txt",
    ],
    tags = ["benchmark"],
    deps = [
        ":day14",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
