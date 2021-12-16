load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day13",
    srcs = ["day13.cc"],
    hdrs = ["day13.h"],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:bfs",
        "//advent_of_code:char_board",
        "//advent_of_code:point",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_library(
    name = "day13_input",
    testonly = 1,
    data = [
        "day13.test",
        "input.txt",
    ],
    deps = [
        ":day13",
    ],
)

cc_test(
    name = "day13_test",
    srcs = ["day13_test.cc"],
    deps = [
        ":day13_input",
        "//advent_of_code/infra:file_test",
    ],
)

cc_library(
    name = "day13_benchmark_lib",
    testonly = 1,
    srcs = ["day13_benchmark.cc"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":day13_input",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day13_benchmark",
    tags = ["benchmark"],
    deps = [
        ":day13_benchmark_lib",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
