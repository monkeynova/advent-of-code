load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day24",
    srcs = ["day24.cc"],
    hdrs = ["day24.h"],
    data = [
        "input.txt",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code:advent_day",
        "@com_google_absl//absl/algorithm:container",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/functional:function_ref",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/strings",
        "@com_googlesource_code_re2//:re2",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day24_test",
    size = "small",
    srcs = ["day24_test.cc"],
    data = [
        "day24.test",
        "input.cshabsin.txt",
    ],
    deps = [
        ":day24",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:file_test",
    ],
)
