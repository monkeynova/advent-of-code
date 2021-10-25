load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day16",
    srcs = ["day16.cc"],
    hdrs = ["day16.h"],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code/2018:vm",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_test(
    name = "day16_test",
    srcs = ["day16_test.cc"],
    args = ["--test_file=advent_of_code/2018/day16/day16.test"],
    data = [
        "day16.test",
        "input.txt",
    ],
    deps = [
        ":day16",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day16_benchmark",
    srcs = ["day16_benchmark.cc"],
    args = ["--test_file=advent_of_code/2018/day16/day16.test"],
    data = [
        "day16.test",
        "input.txt",
    ],
    tags = ["benchmark"],
    deps = [
        ":day16",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
