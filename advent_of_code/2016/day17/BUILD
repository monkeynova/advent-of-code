load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day17",
    srcs = ["day17.cc"],
    hdrs = ["day17.h"],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:bfs",
        "//advent_of_code:md5",
        "//advent_of_code:point",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/strings",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_library(
    name = "day17_input",
    testonly = 1,
    data = [
        "day17.test",
        "input.txt",
    ],
    deps = [
        ":day17",
    ],
)

cc_test(
    name = "day17_test",
    size = "small",
    srcs = ["day17_test.cc"],
    deps = [
        ":day17_input",
        "//advent_of_code/infra:file_test",
    ],
)

cc_library(
    name = "day17_benchmark_lib",
    testonly = 1,
    srcs = ["day17_benchmark.cc"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":day17_input",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day17_benchmark",
    size = "small",
    tags = ["benchmark"],
    deps = [
        ":day17_benchmark_lib",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
