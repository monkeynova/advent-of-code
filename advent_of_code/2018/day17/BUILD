load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day17",
    srcs = ["day17.cc"],
    hdrs = ["day17.h"],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:char_board",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
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
    srcs = ["day17_test.cc"],
    deps = [
        ":day17_input",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day17_benchmark",
    srcs = ["day17_benchmark.cc"],
    tags = ["benchmark"],
    deps = [
        ":day17_input",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
