load("@rules_cc//cc:defs.bzl", "cc_library",)

cc_library(
    name = "assem_bunny",
    srcs = ["assem_bunny.cc"],
    hdrs = ["assem_bunny.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code:advent_day",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
        "@com_googlesource_code_re2//:re2",
    ],
)

