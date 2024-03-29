load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "advent_day",
    srcs = ["advent_day.cc"],
    hdrs = ["advent_day.h"],
    deps = [
        ":tokenizer",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/types:span",
        "@com_monkeynova_gunit_main//:vlog",
    ],
    # Ensure the contained flag(s) get linked.
    alwayslink = 1,
)

cc_library(
    name = "point",
    srcs = ["point.cc"],
    hdrs = ["point.h"],
    deps = [
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/hash",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/strings",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_library(
    name = "point3",
    srcs = ["point3.cc"],
    hdrs = ["point3.h"],
    deps = [
        "@com_google_absl//absl/hash",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/strings",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_library(
    name = "point4",
    hdrs = ["point4.h"],
    deps = [
        "@com_google_absl//absl/hash",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/strings",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_library(
    name = "opt_cmp",
    hdrs = ["opt_cmp.h"],
    deps = [
        "@com_google_absl//absl/types:optional",
    ],
)

cc_library(
    name = "ocr",
    srcs = ["ocr.cc"],
    hdrs = ["ocr.h"],
    deps = [
        ":char_board",
        "@com_google_absl//absl/algorithm:container",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/strings",
        "@com_monkeynova_gunit_main//:vlog",
    ],
)

cc_library(
    name = "bfs",
    hdrs = ["bfs.h"],
    deps = [
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/log:check",
    ],
)

cc_library(
    name = "point_walk",
    hdrs = ["point_walk.h"],
    deps = [
        ":bfs",
        ":point",
        ":point3",
        "@com_google_absl//absl/functional:function_ref",
    ],
)

cc_library(
    name = "char_board",
    srcs = ["char_board.cc"],
    hdrs = ["char_board.h"],
    deps = [
        ":point",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "conway",
    srcs = ["conway.cc"],
    hdrs = ["conway.h"],
    deps = [
        ":char_board",
        ":point",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_monkeynova_gunit_main//:vlog",
    ],
)

cc_library(
    name = "fast_board",
    hdrs = ["fast_board.h"],
    deps = [
        ":char_board",
        ":point",
    ],
)

cc_library(
    name = "md5",
    hdrs = ["md5.h"],
    deps = [
        "//third_party/md5",
    ],
)

cc_library(
    name = "mod",
    srcs = ["mod.cc"],
    hdrs = ["mod.h"],
    deps = [
        "@com_google_absl//absl/functional:function_ref",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/numeric:int128",
        "@com_monkeynova_gunit_main//:vlog",
    ],
)

cc_library(
    name = "directed_graph",
    srcs = ["directed_graph.cc"],
    hdrs = ["directed_graph.h"],
    deps = [
        ":bfs",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/functional:function_ref",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_monkeynova_gunit_main//:vlog",
    ],
)

cc_library(
    name = "interval",
    srcs = ["interval.cc"],
    hdrs = ["interval.h"],
    deps = [
        ":point",
        ":point3",
        "@com_google_absl//absl/functional:function_ref",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/strings",
        "@com_monkeynova_gunit_main//:vlog",
    ],
)

cc_library(
    name = "loop_history",
    hdrs = ["loop_history.h"],
    deps = [
        "@com_google_absl//absl/container:node_hash_map",
    ],
)

cc_library(
    name = "tokenizer",
    srcs = ["tokenizer.cc"],
    hdrs = ["tokenizer.h"],
    deps = [
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_monkeynova_gunit_main//:vlog",
    ],
)

cc_library(
    name = "splice_ring",
    hdrs = ["splice_ring.h"],
    deps = [
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/log",
        "@com_monkeynova_gunit_main//:vlog",
    ],
    alwayslink = 1,
)

cc_library(
    name = "advent_of_code",
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//advent_of_code/2015",
        "//advent_of_code/2016",
        "//advent_of_code/2017",
        "//advent_of_code/2018",
        "//advent_of_code/2019",
        "//advent_of_code/2020",
        "//advent_of_code/2021",
        "//advent_of_code/2022",
        "//advent_of_code/2023",
    ],
)
