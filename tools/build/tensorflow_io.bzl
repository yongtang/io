load("@bazel_tools//tools/build_defs/repo:utils.bzl", "patch", "workspace_and_buildfile")

def tf_io_copts():
    return (
        select({
            "@bazel_tools//src/conditions:windows": [
                "/DEIGEN_STRONG_INLINE=inline",
                "-DTENSORFLOW_MONOLITHIC_BUILD",
                "/DPLATFORM_WINDOWS",
                "/DEIGEN_HAS_C99_MATH",
                "/DTENSORFLOW_USE_EIGEN_THREADPOOL",
                "/DEIGEN_AVOID_STL_ARRAY",
                "/Iexternal/gemmlowp",
                "/wd4018",
                "/wd4577",
                "/DNOGDI",
                "/UTF_COMPILE_LIBRARY",
                "/DNDEBUG",
            ],
            "@bazel_tools//src/conditions:darwin": [
                "-DNDEBUG",
            ],
            "//conditions:default": [
                "-DNDEBUG",
                "-pthread",
            ],
        })
    )

def _tensorflow_archive_impl(ctx):
    tensorflow_commit = ctx.os.environ.get("TENSORFLOW_COMMIT")
    tensorflow_sha256 = ctx.os.environ.get("TENSORFLOW_SHA256", "")

    print("TENSORFLOW COMMIT: {}".format(tensorflow_commit))
    print("TENSORFLOW SHA256: {}".format(tensorflow_sha256))

    ctx.download_and_extract(
        [url.format(commit = tensorflow_commit) for url in ctx.attr.urls],
        "",
        ctx.attr.sha256.format(sha256 = tensorflow_sha256),
        ctx.attr.type,
        ctx.attr.strip_prefix.format(commit = tensorflow_commit),
    )
    workspace_and_buildfile(ctx)
    patch(ctx)

tensorflow_archive = repository_rule(
    attrs = {
        "urls": attr.string_list(),
        "sha256": attr.string(),
        "strip_prefix": attr.string(),
        "type": attr.string(),
        "patches": attr.label_list(),
        "patch_tool": attr.string(default = ""),
        "patch_args": attr.string_list(default = ["-p0"]),
        "patch_cmds": attr.string_list(),
        "patch_cmds_win": attr.string_list(),
        "build_file": attr.label(),
        "build_file_content": attr.string(),
        "workspace_file": attr.label(),
        "workspace_file_content": attr.string(),
    },
    implementation = _tensorflow_archive_impl,
    environ = [
        "TENSORFLOW_COMMIT",
        "TENSORFLOW_SHA256",
    ],
)

def _llvm_archive_impl(ctx):
    llvm_commit = ctx.os.environ["LLVM_COMMIT"]
    llvm_sha256 = ctx.os.environ["LLVM_SHA256"]

    print("LLVM COMMIT: {}".format(llvm_commit))
    print("LLVM SHA256: {}".format(llvm_sha256))

    ctx.download_and_extract(
        [url.format(commit = llvm_commit) for url in ctx.attr.urls],
        "",
        ctx.attr.sha256.format(sha256 = llvm_sha256),
        ctx.attr.type,
        ctx.attr.strip_prefix.format(commit = llvm_commit),
    )
    workspace_and_buildfile(ctx)
    patch(ctx)

    for internal_src, external_dest in ctx.attr.additional_build_files.items():
        ctx.symlink(Label(internal_src), ctx.path(external_dest))

llvm_archive = repository_rule(
    attrs = {
        "urls": attr.string_list(),
        "sha256": attr.string(),
        "strip_prefix": attr.string(),
        "type": attr.string(),
        "patches": attr.label_list(),
        "patch_tool": attr.string(default = ""),
        "patch_args": attr.string_list(default = ["-p0"]),
        "patch_cmds": attr.string_list(),
        "patch_cmds_win": attr.string_list(),
        "build_file": attr.label(),
        "build_file_content": attr.string(),
        "workspace_file": attr.label(),
        "workspace_file_content": attr.string(),
        "additional_build_files": attr.string_dict(),
    },
    implementation = _llvm_archive_impl,
    environ = [
        "LLVM_COMMIT",
        "LLVM_SHA256",
    ],
)
