workspace(name="com_monkeynova_advent_of_code")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

git_repository(
    name = "bazel_skylib",
    remote = "https://github.com/bazelbuild/bazel-skylib.git",
    # branch = "main",
    commit = "652c8f0b2817daaa2570b7a3b2147643210f7dc7",
)
load("@bazel_skylib//:workspace.bzl", "bazel_skylib_workspace")
bazel_skylib_workspace()

git_repository(
   name = "com_monkeynova_base_workspace",
   remote = "https://github.com/monkeynova/base-workspace.git",
   # branch = "main",
   commit = "079048a672da87d2cede89febac49f54a5cbb165",
)

git_repository(
    name = "com_google_googletest",
    remote = "https://github.com/google/googletest.git",
    # branch = "main",
    commit = "116b7e55281c4200151524b093ecc03757a4ffda",
)

git_repository(
    name = "com_google_absl",
    remote = "https://github.com/abseil/abseil-cpp.git",
    commit = "a0b72adc3576eb0b77efb7133207c354d0adb4bc",
)

git_repository(
    name = "com_github_google_benchmark",
    remote = "https://github.com/google/benchmark.git",
    # branch = "main",
    commit = "7495f83e2a6e1aa69592fcda6e5c6c1b0b4fa118",
)

git_repository(
    name = "com_googlesource_code_re2",
    remote = "https://github.com/google/re2.git",
    # branch = "abseil"
    commit = "578843a516fd1da7084ae46209a75f3613b6065e",
)

git_repository(
    name = "com_google_file_based_test_driver",
    remote = "https://github.com/google/file-based-test-driver.git",
    commit = "fd7661b168f640f68da39f97dad26e426eb6c339",
)

git_repository(
    name = "com_monkeynova_gunit_main",
    remote = "https://github.com/monkeynova/gunit-main.git",
    # branch = "main"
    commit = "8b0dec052e1aacd71f91aade4530aae6a452b0e5",
)

git_repository(
    name = "aspect_bazel_lib",
    remote = "https://github.com/aspect-build/bazel-lib.git",
    # branch = "main",
    commit = "8cf7e6f995b43641f41e9dd9a6b314684a7c0c9a",
)

load("@com_google_googletest//:googletest_deps.bzl", "googletest_deps")
googletest_deps()

load("@com_github_google_benchmark//:bazel/benchmark_deps.bzl", "benchmark_deps")
benchmark_deps()
