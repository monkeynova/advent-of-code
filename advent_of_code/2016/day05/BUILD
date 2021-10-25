load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day05",
    srcs = ["day05.cc"],
    hdrs = ["day05.h"],
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
    name = "day05_test",
    srcs = ["day05_test.cc"],
    args = ["--test_file=advent_of_code/2016/day05/day05.test"],
    data = [
        "day05.test",
        "input.txt",
    ],
    deps = [
        ":day05",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day05_benchmark",
    srcs = ["day05_benchmark.cc"],
    args = ["--test_file=advent_of_code/2016/day05/day05.test"],
    data = [
        "day05.test",
        "input.txt",
    ],
    tags = ["benchmark"],
    deps = [
        ":day05",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
