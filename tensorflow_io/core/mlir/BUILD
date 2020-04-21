load("@org_tensorflow//third_party/mlir:tblgen.bzl", "gentbl")

package(
    default_visibility = ["//visibility:public"],
    licenses = ["notice"],  # Apache 2.0
)

filegroup(
    name = "tfio_ops_td_files",
    srcs = [
        "ir/tfio_ops.td",
        "@llvm-project//mlir:OpBaseTdFiles",
    ],
)

gentbl(
    name = "tfio_inc_gen",
    tbl_outs = [
        (
            "-gen-op-decls",
            "ir/tfio_ops.h.inc",
        ),
        (
            "-gen-op-defs",
            "ir/tfio_ops.cc.inc",
        ),
        (
            "-gen-dialect-decls",
            "ir/tfio_dialect.h.inc",
        ),
        (
            "-gen-dialect-doc",
            "g3doc/tfio_dialect.md",
        ),
    ],
    tblgen = "@llvm-project//mlir:mlir-tblgen",
    td_file = "ir/tfio_ops.td",
    td_srcs = [
        "ir/tfio_ops.td",
        "@llvm-project//mlir:OpBaseTdFiles",
        "@llvm-project//mlir:include/mlir/Interfaces/LoopLikeInterface.td",
        "@llvm-project//mlir:include/mlir/Interfaces/SideEffects.td",
    ],
)

cc_library(
    name = "tensorflow_io",
    srcs = [
        "ir/tfio_dialect.h.inc",
        "ir/tfio_ops.cc",
        "ir/tfio_ops.cc.inc",
        "ir/tfio_ops.h.inc",
    ],
    hdrs = [
        "ir/tfio_ops.h",
    ],
    deps = [
        ":tfio_inc_gen",
        "@llvm-project//llvm:support",
        "@llvm-project//mlir:Dialect",
        "@llvm-project//mlir:IR",
        "@llvm-project//mlir:SideEffects",
        "@llvm-project//mlir:StandardOps",
        "@llvm-project//mlir:Support",
        "@llvm-project//mlir:TransformUtils",
    ],
    alwayslink = 1,
)

cc_library(
    name = "tensorflow_io_dialect_registration",
    srcs = [
        "ir/dialect_registration.cc",
    ],
    deps = [
        ":tensorflow_io",
    ],
    alwayslink = 1,
)
