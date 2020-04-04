#!/usr/bin/env python
# encoding: utf-8

import sys
import os
import fnmatch
import glob

sys.path.insert(0, sys.path[0] + "/waf_tools")

VERSION = "1.0.0"
APPNAME = "quad-arena"

srcdir = "."
blddir = "build"

from waflib.Tools import waf_unit_test
from waflib import Logs
from waflib.Build import BuildContext
import magnum


def options(opt):
    # Load modules necessary in the configuration function
    opt.load("compiler_cxx")
    opt.load("compiler_c")
    opt.load("corrade")
    opt.load("magnum")


def configure(cfg):
    # Load modules defined in the option function
    cfg.load("compiler_cxx")
    cfg.load("compiler_c")
    cfg.load("corrade")
    cfg.load("magnum")

    cfg.check_corrade(components="Utility PluginManager", required=True)
    cfg.check_magnum(
        components="MeshTools Shaders Sdl2Application SceneGraph Primitives Trade",
        required=True,
    )

    # Magical flags definition for different compilers from Konst... hard to say what eachone does
    if cfg.env.CXX_NAME in ["icc", "icpc"]:
        common_flags = "-Wall -std=c++14"
        opt_flags = " -O3 -xHost -mtune=native -unroll -g"
    elif cfg.env.CXX_NAME in ["clang"]:
        common_flags = "-Wall -std=c++14"
        opt_flags = " -O3 -march=native -g -faligned-new"
    else:
        gcc_version = int(cfg.env["CC_VERSION"][0] + cfg.env["CC_VERSION"][1])
        if gcc_version < 47:
            common_flags = "-Wall -std=c++0x"
            # cfg.fatal("Compiler should support C++14")
        else:
            common_flags = "-Wall -std=c++14"
        opt_flags = " -O3 -march=native -g"
        if gcc_version >= 71:
            opt_flags = opt_flags + " -faligned-new"

    all_flags = common_flags + opt_flags
    cfg.env["CXXFLAGS"] = cfg.env["CXXFLAGS"] + all_flags.split(" ")
    print(cfg.env["CXXFLAGS"])


def build(bld):
    # Check platform
    suffix = "dylib" if bld.env["DEST_OS"] == "darwin" else "so"

    # Define sources files
    files = []
    for root, dirnames, filenames in os.walk(bld.path.abspath() + "/src/"):
        for filename in fnmatch.filter(filenames, "*.cpp"):
            files.append(os.path.join(root, filename))

    files = [f[len(bld.path.abspath()) + 1 :] for f in files]
    srcs = " ".join(files)

    libs = magnum.get_magnum_dependency_libs(
        bld, "Sdl2Application Shaders Primitives MeshTools SceneGraph Trade"
    )

    print(libs)

    # Build examples
    bld.program(
        features="cxx",
        install_path=None,
        source=srcs,
        includes="./src",
        uselib=libs,
        framework=["Foundation", "OpenGL"],
        target="test",
    )

