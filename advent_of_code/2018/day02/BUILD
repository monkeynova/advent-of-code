load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day02",
    srcs = ["day02.cc"],
    hdrs = ["day02.h"],
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
    name = "day02_test",
    srcs = ["day02_test.cc"],
    args = ["--test_file=advent_of_code/2018/day02/day02.test"],
    data = [
        "day02.test",
        "input.txt",
    ],
    deps = [
        ":day02",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day02_benchmark",
    srcs = ["day02_benchmark.cc"],
    args = ["--test_file=advent_of_code/2018/day02/day02.test"],
    data = [
        "day02.test",
        "input.txt",
    ],
    tags = ["benchmark"],
    deps = [
        ":day02",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
