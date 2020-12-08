load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day9",
    srcs = ["day9.cc"],
    hdrs = ["day9.h"],
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
    name = "day9_test",
    srcs = ["day9_test.cc"],
    args = ["--test_file=advent_of_code/2020/day9/day9.test"],
    data = [
        "day9.test",
        "input.txt",
    ],
    deps = [
        ":day9",
        "//advent_of_code:file_test",
    ],
)

cc_test(
    name = "day9_benchmark",
    srcs = ["day9_benchmark.cc"],
    args = ["--test_file=advent_of_code/2020/day9/day9.test"],
    data = [
        "day9.test",
        "input.txt",
    ],
    deps = [
        ":day9",
        "//advent_of_code:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
