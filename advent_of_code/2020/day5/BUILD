load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day5",
    srcs = ["day5.cc"],
    hdrs = ["day5.h"],
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
    name = "day5_test",
    srcs = ["day5_test.cc"],
    args = ["--test_file=advent_of_code/2020/day5/day5.test"],
    data = [
        "day5.test",
        "input.txt",
    ],
    deps = [
        ":day5",
        "//advent_of_code:file_test",
    ],
)

cc_test(
    name = "day5_benchmark",
    srcs = ["day5_benchmark.cc"],
    args = ["--test_file=advent_of_code/2020/day5/day5.test"],
    data = [
        "day5.test",
        "input.txt",
    ],
    deps = [
        ":day5",
        "//advent_of_code:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
