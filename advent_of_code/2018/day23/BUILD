load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day23",
    srcs = ["day23.cc"],
    hdrs = ["day23.h"],
    deps = [
        "//advent_of_code:advent_day",
        "//advent_of_code:point",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
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
    srcs = ["day23_test.cc"],
    deps = [
        ":day23_input",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "day23_benchmark",
    srcs = ["day23_benchmark.cc"],
    tags = ["benchmark"],
    deps = [
        ":day23_input",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
