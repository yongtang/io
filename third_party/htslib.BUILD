# Description:
# C library for high-throughput sequencing data formats
#

licenses(["notice"])  # MIT/Expat, cram uses BSD

exports_files(["LICENSE"])

version = "1.9"

# Generated by running
# ./configure --disable-libcurl --disable-gcs --disable-s3
genrule(
    name = "config_h",
    outs = ["config.h"],
    cmd = """
        exec > "$@"
        echo '#define HAVE_DRAND48 1'
        echo '#define HAVE_FDATASYNC 1'
        echo '#define HAVE_FSEEKO 1'
        echo '#define HAVE_FSYNC 1'
        echo '#define HAVE_GETPAGESIZE 1'
        echo '#define HAVE_GMTIME_R 1'
        echo '#define HAVE_INTTYPES_H 1'
        echo '#define HAVE_LIBBZ2 1'
        echo '#define HAVE_LIBLZMA 1'
        echo '#define HAVE_LIBZ 1'
        echo '#define HAVE_MEMORY_H 1'
        echo '#define HAVE_MMAP 1'
        echo '#define HAVE_STDINT_H 1'
        echo '#define HAVE_STDLIB_H 1'
        echo '#define HAVE_STRING_H 1'
        echo '#define HAVE_STRINGS_H 1'
        echo '#define HAVE_SYS_PARAM_H 1'
        echo '#define HAVE_SYS_STAT_H 1'
        echo '#define HAVE_SYS_TYPES_H 1'
        echo '#define HAVE_UNISTD_H 1'
        echo '#define PACKAGE_BUGREPORT "samtools-help@lists.sourceforge.net"'
        echo '#define PACKAGE_NAME "HTSlib"'
        echo '#define PACKAGE_STRING "HTSlib %s"'
        echo '#define PACKAGE_TARNAME "htslib"'
        echo '#define PACKAGE_URL "http://www.htslib.org/"'
        echo '#define PACKAGE_VERSION "%s"'
        echo '#define PLUGIN_EXT ".so"'
        echo '#define STDC_HEADERS 1'
    """ % (version, version),
)

genrule(
    name = "version",
    outs = ["version.h"],
    cmd = """echo '#define HTS_VERSION "%s"' > "$@" """ % version,
)

genrule(
    name = "strings",
    outs = ["strings.h"],
    cmd = """
        exec > "$@"
        echo '#include <windows.h>'
        echo 'typedef __int64 ssize_t;'
        echo '#define F_OK 0'
        echo '#define W_OK 2'
        echo '#define R_OK 4'
    """,
)

# Vanilla htslib, no extensions.
cc_library(
    name = "htslib",
    srcs = glob([
        "htslib/*.h",
        "cram/*.h",
    ]) + [
        "bcf_sr_sort.c",
        "bcf_sr_sort.h",
        "bgzf.c",
        "cram/cram_codecs.c",
        "cram/cram_decode.c",
        "cram/cram_encode.c",
        "cram/cram_external.c",
        "cram/cram_index.c",
        "cram/cram_io.c",
        "cram/cram_samtools.c",
        "cram/cram_stats.c",
        "cram/files.c",
        "cram/mFILE.c",
        "cram/open_trace_file.c",
        "cram/pooled_alloc.c",
        "cram/rANS_static.c",
        "cram/sam_header.c",
        "cram/string_alloc.c",
        "errmod.c",
        "faidx.c",
        "hfile.c",
        "hfile_net.c",
        "hts.c",
        "hts_os.c",
        #"kfunc.c",
        "knetfile.c",
        "kstring.c",
        "md5.c",
        "multipart.c",
        "plugin.c",
        "probaln.c",
        "realn.c",
        "regidx.c",
        "sam.c",
        "synced_bcf_reader.c",
        "tbx.c",
        "textutils.c",
        "thread_pool.c",
        "vcf.c",
        "vcf_sweep.c",
        "vcfutils.c",
    ] + [
        "os/lzma_stub.h",
        "hfile_internal.h",
        "hts_internal.h",
        "textutils_internal.h",
        "thread_pool_internal.h",
    ],
    # Genrules in lieu of ./configure.  Minimum viable linux.
    hdrs = [
        "config.h",
        "version.h",
    ] + select({
        "@bazel_tools//src/conditions:windows": [
            "strings.h",
        ],
        "//conditions:default": [],
    }),
    copts = select({
        "@bazel_tools//src/conditions:windows": [],
        "//conditions:default": [
            "-Wno-implicit-function-declaration",  # cram_io.c
            "-Wno-unused-variable",  # cram_encode.c
            "-Wno-error",
        ],
    }),
    includes = ["."],
    visibility = ["//visibility:public"],
    defines = [
        "WIN32_LEAN_AND_MEAN",
    ],
    deps = [
        "@bzip2",
        "@xz//:lzma",
        "@zlib",
    ] + select({
        "@bazel_tools//src/conditions:windows": [
            "@pthreads4w",
            "@postgresql",
        ],
        "//conditions:default": [],
    }),
)
