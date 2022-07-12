load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
  name = "test",
  srcs = glob(["*.cpp"]),
  deps = [
    "//test_server/protocol:protocol",
    "//ironman/serialize:serialize",
    "@com_google_googletest//:gtest_main",
  ],
)
