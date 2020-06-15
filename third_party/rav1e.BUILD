# Description:
#   rav1e library

licenses(["notice"])  # BSD license

exports_files(["LICENSE"])

load(
    "@io_bazel_rules_rust//rust:rust.bzl",
    "rust_binary",
    "rust_library",
    "rust_doc",
)

rust_library(
    name = "rav1e_rust",
    srcs = [
        "src/api/util.rs",
    ],
    crate_features = ["default"],
    rustc_flags = ["--cap-lints=allow"],
)

cc_library(
    name = "rav1e",
    visibility = ["//visibility:public"],
    deps = [
        ":rav1e_rust",
    ],
)
