load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day21",
    srcs = ["day21.cc"],
    hdrs = ["day21.h"],
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
    name = "day21_test",
    srcs = ["day21_test.cc"],
    args = ["--test_file=advent_of_code/2018/day21/day21.test"],
    data = [
        "day21.test",
        "input.txt",
    ],
    deps = [
        ":day21",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day21_benchmark",
    srcs = ["day21_benchmark.cc"],
    args = ["--test_file=advent_of_code/2018/day21/day21.test"],
    data = [
        "day21.test",
        "input.txt",
    ],
    deps = [
        ":day21",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
