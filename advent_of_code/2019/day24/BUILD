load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day24",
    srcs = ["day24.cc"],
    hdrs = ["day24.h"],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:char_board",
        "//advent_of_code:opt_cmp",
        "//advent_of_code:point",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_library(
    name = "day24_input",
    testonly = 1,
    data = [
        "day24.test",
        "input.txt",
    ],
    deps = [
        ":day24",
    ],
)

cc_test(
    name = "day24_test",
    srcs = ["day24_test.cc"],
    deps = [
        ":day24_input",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day24_benchmark",
    srcs = ["day24_benchmark.cc"],
    tags = ["benchmark"],
    deps = [
        ":day24_input",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
