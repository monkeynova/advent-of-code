load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day01",
    srcs = ["day01.cc"],
    hdrs = ["day01.h"],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:point",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_test(
    name = "day01_test",
    srcs = ["day01_test.cc"],
    args = ["--test_file=advent_of_code/2016/day01/day01.test"],
    data = [
        "input.txt",
        "day01.test",
    ],
    deps = [
        ":day01",
        "//advent_of_code:file_test",
    ],
)

cc_test(
    name = "day01_benchmark",
    srcs = ["day01_benchmark.cc"],
    args = ["--test_file=advent_of_code/2016/day01/day01.test"],
    data = [
        "input.txt",
        "day01.test",
    ],
    deps = [
        ":day01",
        "//advent_of_code:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
