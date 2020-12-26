load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day13",
    srcs = ["day13.cc"],
    hdrs = ["day13.h"],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:point",
        "//advent_of_code/2019:int_code",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
    ],
)

cc_test(
    name = "day13_test",
    srcs = ["day13_test.cc"],
    args = ["--test_file=advent_of_code/2019/day13/day13.test"],
    data = [
        "day13.test",
        "input.txt",
    ],
    deps = [
        ":day13",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day13_benchmark",
    srcs = ["day13_benchmark.cc"],
    args = ["--test_file=advent_of_code/2019/day13/day13.test"],
    data = [
        "day13.test",
        "input.txt",
    ],
    deps = [
        ":day13",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
