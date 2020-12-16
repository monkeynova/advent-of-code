load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day17",
    srcs = ["day17.cc"],
    hdrs = ["day17.h"],
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
    name = "day17_test",
    srcs = ["day17_test.cc"],
    args = ["--test_file=advent_of_code/2020/day17/day17.test"],
    data = [
        "input.txt",
        "day17.test",
    ],
    deps = [
        ":day17",
        "//advent_of_code:file_test",
    ],
)

cc_test(
    name = "day17_benchmark",
    srcs = ["day17_benchmark.cc"],
    args = ["--test_file=advent_of_code/2020/day17/day17.test"],
    data = [
        "input.txt",
        "day17.test",
    ],
    deps = [
        ":day17",
        "//advent_of_code:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
