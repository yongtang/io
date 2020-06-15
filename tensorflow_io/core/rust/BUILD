package(default_visibility = ["//visibility:public"])

load(
    "@io_bazel_rules_rust//rust:rust.bzl",
    "rust_binary",
    "rust_library",
    "rust_doc",
)

rust_library(
    name = "hello_lib",
    srcs = [
        "src/greeter.rs",
        "src/lib.rs",
    ],
    crate_features = ["default"],
    rustc_flags = ["--cap-lints=allow"],
)


rust_binary(
    name = "hello_world",
    srcs = ["src/main.rs"],
    deps = [":hello_lib"],
)

rust_doc(
    name = "hello_world_doc",
    dep = ":hello_world",
)
