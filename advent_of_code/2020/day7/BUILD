load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day7",
    srcs = ["day7.cc"],
    hdrs = ["day7.h"],
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
    name = "day7_test",
    srcs = ["day7_test.cc"],
    args = ["--test_file=advent_of_code/2020/day7/day7.test"],
    data = [
        "input.txt",
        "day7.test",
    ],
    deps = [
        ":day7",
        "//advent_of_code:file_test",
    ],
)

cc_test(
    name = "day7_benchmark",
    srcs = ["day7_benchmark.cc"],
    args = ["--test_file=advent_of_code/2020/day7/day7.test"],
    data = [
        "input.txt",
        "day7.test",
    ],
    deps = [
        ":day7",
        "//advent_of_code:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
