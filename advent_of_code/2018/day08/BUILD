load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day08",
    srcs = ["day08.cc"],
    hdrs = ["day08.h"],
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
    name = "day08_test",
    srcs = ["day08_test.cc"],
    args = ["--test_file=advent_of_code/2018/day08/day08.test"],
    data = [
        "day08.test",
        "input.txt",
    ],
    deps = [
        ":day08",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day08_benchmark",
    srcs = ["day08_benchmark.cc"],
    args = ["--test_file=advent_of_code/2018/day08/day08.test"],
    data = [
        "day08.test",
        "input.txt",
    ],
    deps = [
        ":day08",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
