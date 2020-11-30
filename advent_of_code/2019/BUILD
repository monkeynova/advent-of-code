load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "int_code",
    srcs = ["int_code.cc"],
    hdrs = ["int_code.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
    ],
)
