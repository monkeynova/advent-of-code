load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day06",
    srcs = ["day06.cc"],
    hdrs = ["day06.h"],
    deps = [
        "//advent_of_code:advent_day",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_test(
    name = "day06_test",
    srcs = ["day06_test.cc"],
    args = ["--test_file=advent_of_code/2019/day06/day06.test"],
    data = [
        "day06.test",
        "input.txt",
    ],
    deps = [
        ":day06",
        "//advent_of_code:file_test",
    ],
)

cc_test(
    name = "day06_benchmark",
    srcs = ["day06_benchmark.cc"],
    args = ["--test_file=advent_of_code/2019/day06/day06.test"],
    data = [
        "day06.test",
        "input.txt",
    ],
    deps = [
        ":day06",
        "//advent_of_code:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
