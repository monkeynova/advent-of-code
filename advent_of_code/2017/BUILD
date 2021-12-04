load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "knot_hash",
    srcs = ["knot_hash.cc"],
    hdrs = ["knot_hash.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/types:span",
        "@com_google_glog//:glog",
    ],
)

cc_test(
    name = "knot_hash_test",
    srcs = ["knot_hash_test.cc"],
    deps = [
        ":knot_hash",
        "@com_google_googletest//:gtest",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
