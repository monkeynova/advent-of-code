load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day8",
    srcs = ["day8.cc"],
    hdrs = ["day8.h"],
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
    name = "day8_test",
    srcs = ["day8_test.cc"],
    args = ["--test_file=advent_of_code/2020/day8/day8.test"],
    data = [
        "day8.test",
        "input.txt",
    ],
    deps = [
        ":day8",
        "//advent_of_code:file_test",
    ],
)

cc_test(
    name = "day8_benchmark",
    srcs = ["day8_benchmark.cc"],
    args = ["--test_file=advent_of_code/2020/day8/day8.test"],
    data = [
        "day8.test",
        "input.txt",
    ],
    deps = [
        ":day8",
        "//advent_of_code:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
