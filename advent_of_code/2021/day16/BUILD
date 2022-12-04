load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "bits_expr",
    srcs = ["bits_expr.cc"],
    hdrs = ["bits_expr.h"],
    deps = [
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/log",
    ],
)

cc_test(
    name = "bits_expr_test",
    srcs = ["bits_expr_test.cc"],
    deps = [
        ":bits_expr",
        # Allow file flag options on all tests to avoid test failures in
        # bazelisk test ... --test_arg=<file arg>.
        "//advent_of_code/infra:file_flags",
        "//advent_of_code/infra:gunit_main",
    ],
)

cc_library(
    name = "day16",
    srcs = ["day16.cc"],
    hdrs = ["day16.h"],
    deps = [
        ":bits_expr",
        "//advent_of_code:advent_day",
        "@com_google_absl//absl/algorithm:container",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/log",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_library(
    name = "day16_input",
    testonly = 1,
    data = [
        "day16.test",
        "input.txt",
    ],
    deps = [
        ":day16",
    ],
)

cc_test(
    name = "day16_test",
    srcs = ["day16_test.cc"],
    deps = [
        ":day16_input",
        "//advent_of_code/infra:file_test",
    ],
)

cc_library(
    name = "day16_benchmark_lib",
    testonly = 1,
    srcs = ["day16_benchmark.cc"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":day16_input",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:gunit_main",
    ],
    alwayslink = 1,
)

cc_test(
    name = "day16_benchmark",
    tags = ["benchmark"],
    deps = [
        ":day16_benchmark_lib",
        "//advent_of_code/infra:gunit_main",
    ],
)
