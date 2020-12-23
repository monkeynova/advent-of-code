load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day04",
    srcs = ["day04.cc"],
    hdrs = ["day04.h"],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:md5",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_test(
    name = "day04_test",
    srcs = ["day04_test.cc"],
    args = ["--test_file=advent_of_code/2015/day04/day04.test"],
    data = [
        "day04.test",
        "input.txt",
    ],
    deps = [
        ":day04",
        "//advent_of_code:file_test",
    ],
)

cc_test(
    name = "day04_benchmark",
    srcs = ["day04_benchmark.cc"],
    args = ["--test_file=advent_of_code/2015/day04/day04.test"],
    data = [
        "day04.test",
        "input.txt",
    ],
    deps = [
        ":day04",
        "//advent_of_code:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
