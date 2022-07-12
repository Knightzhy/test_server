load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
  name = "test",
  srcs = ["test_thread.cpp"],
  linkopts = ["-pthread"],
)

cc_binary(
  name = "test2",
  srcs = ["test_thread2.cpp"],
)