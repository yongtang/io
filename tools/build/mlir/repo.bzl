def _tf_http_archive(ctx):
    ctx.download_and_extract(
        ctx.attr.urls,
        "",
        ctx.attr.sha256,
        ctx.attr.type,
        ctx.attr.strip_prefix,
    )
    if ctx.attr.additional_build_files:
        for internal_src, external_dest in ctx.attr.additional_build_files.items():
            ctx.symlink(Label(internal_src), ctx.path(external_dest))

tf_http_archive = repository_rule(
    attrs = {
        "sha256": attr.string(mandatory = True),
        "urls": attr.string_list(
            mandatory = True,
            allow_empty = False,
        ),
        "strip_prefix": attr.string(),
        "type": attr.string(),
        "additional_build_files": attr.string_dict(),
    },
    implementation = _tf_http_archive,
)

