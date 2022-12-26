load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day23",
    srcs = ["day23.cc"],
    hdrs = ["day23.h"],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:bfs",
        "//advent_of_code:char_board",
        "@com_google_absl//absl/algorithm:container",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/container:node_hash_set",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/strings",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_library(
    name = "day23_input",
    testonly = 1,
    data = [
        "day23.test",
        "input.txt",
    ],
    deps = [
        ":day23",
    ],
)

cc_test(
    name = "day23_test",
    size = "small",
    srcs = ["day23_test.cc"],
    deps = [
        ":day23_input",
        "//advent_of_code/infra:file_test",
    ],
)

cc_library(
    name = "day23_benchmark_lib",
    testonly = 1,
    srcs = ["day23_benchmark.cc"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":day23_input",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day23_benchmark",
    size = "small",
    tags = ["benchmark"],
    deps = [
        ":day23_benchmark_lib",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
