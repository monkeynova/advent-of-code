load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day25",
    srcs = ["day25.cc"],
    hdrs = ["day25.h"],
    data = [
        "input.txt",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:tokenizer",
        "@abseil-cpp//absl/algorithm:container",
        "@abseil-cpp//absl/container:flat_hash_map",
        "@abseil-cpp//absl/log",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day25_test",
    size = "small",
    srcs = ["day25_test.cc"],
    data = [
        "day25.test",
    ],
    deps = [
        ":day25",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:file_test",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
