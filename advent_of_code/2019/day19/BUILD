load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day19",
    srcs = ["day19.cc"],
    hdrs = ["day19.h"],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:char_board",
        "//advent_of_code:point",
        "//advent_of_code/2019:int_code",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_test(
    name = "day19_test",
    srcs = ["day19_test.cc"],
    args = ["--test_file=advent_of_code/2019/day19/day19.test"],
    data = [
        "day19.test",
        "input.txt",
    ],
    deps = [
        ":day19",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day19_benchmark",
    srcs = ["day19_benchmark.cc"],
    args = ["--test_file=advent_of_code/2019/day19/day19.test"],
    data = [
        "day19.test",
        "input.txt",
    ],
    tags = ["benchmark"],
    deps = [
        ":day19",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
