load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day22",
    srcs = ["day22.cc"],
    hdrs = ["day22.h"],
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
    name = "day22_test",
    srcs = ["day22_test.cc"],
    args = ["--test_file=advent_of_code/2018/day22/day22.test"],
    data = [
        "day22.test",
        "input.txt",
    ],
    deps = [
        ":day22",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day22_benchmark",
    srcs = ["day22_benchmark.cc"],
    args = ["--test_file=advent_of_code/2018/day22/day22.test"],
    data = [
        "day22.test",
        "input.txt",
    ],
    deps = [
        ":day22",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
