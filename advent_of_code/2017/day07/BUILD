load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day07",
    srcs = ["day07.cc"],
    hdrs = ["day07.h"],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:directed_graph",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_test(
    name = "day07_test",
    srcs = ["day07_test.cc"],
    args = ["--test_file=advent_of_code/2017/day07/day07.test"],
    data = [
        "day07.test",
        "input.txt",
    ],
    deps = [
        ":day07",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day07_benchmark",
    srcs = ["day07_benchmark.cc"],
    args = ["--test_file=advent_of_code/2017/day07/day07.test"],
    data = [
        "day07.test",
        "input.txt",
    ],
    deps = [
        ":day07",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
