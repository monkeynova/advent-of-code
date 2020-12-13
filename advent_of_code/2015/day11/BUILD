load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day11",
    srcs = ["day11.cc"],
    hdrs = ["day11.h"],
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
    name = "day11_test",
    srcs = ["day11_test.cc"],
    args = ["--test_file=advent_of_code/2015/day11/day11.test"],
    data = [
        "input.txt",
        "day11.test",
    ],
    deps = [
        ":day11",
        "//advent_of_code:file_test",
    ],
)

cc_test(
    name = "day11_benchmark",
    srcs = ["day11_benchmark.cc"],
    args = ["--test_file=advent_of_code/2015/day11/day11.test"],
    data = [
        "input.txt",
        "day11.test",
    ],
    deps = [
        ":day11",
        "//advent_of_code:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
