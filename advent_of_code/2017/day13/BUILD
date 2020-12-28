load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day13",
    srcs = ["day13.cc"],
    hdrs = ["day13.h"],
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
    name = "day13_test",
    srcs = ["day13_test.cc"],
    args = ["--test_file=advent_of_code/2017/day13/day13.test"],
    data = [
        "input.txt",
        "day13.test",
    ],
    deps = [
        ":day13",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day13_benchmark",
    srcs = ["day13_benchmark.cc"],
    args = ["--test_file=advent_of_code/2017/day13/day13.test"],
    data = [
        "input.txt",
        "day13.test",
    ],
    deps = [
        ":day13",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
