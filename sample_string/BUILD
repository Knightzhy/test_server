load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
  name = "test",
  srcs = glob(["*.cpp"]),
  deps = [
    "@com_google_googletest//:gtest_main",
  ],
  copts = ["-std=c++11"],
)
