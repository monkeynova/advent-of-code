load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day03",
    srcs = ["day03.cc"],
    hdrs = ["day03.h"],
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
    name = "day03_test",
    srcs = ["day03_test.cc"],
    args = ["--test_file=advent_of_code/2016/day03/day03.test"],
    data = [
        "day03.test",
        "input.txt",
    ],
    deps = [
        ":day03",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day03_benchmark",
    srcs = ["day03_benchmark.cc"],
    args = ["--test_file=advent_of_code/2016/day03/day03.test"],
    data = [
        "day03.test",
        "input.txt",
    ],
    deps = [
        ":day03",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
