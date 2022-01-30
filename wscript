#!/usr/bin/env python
# encoding: utf-8

import os
import os.path as osp
import fnmatch

VERSION = "1.0.0"
APPNAME = "graphics-lib"

srcdir = "."
blddir = "build"

# Tools' name and directory
tools = {"utilslib": ""}


def options(opt):
    # Load C++ compiler options
    opt.load("compiler_cxx")

    # Load personal tools options
    for key in tools:
        opt.load(key, tooldir=os.path.join(
            tools[key], "share/waf"))

    # Load external tools options
    opt.load("flags magnum", tooldir="waf_tools")

    # Add options
    opt.add_option(
        "--shared", action="store_true", help="build shared library", dest="shared"
    )
    opt.add_option(
        "--static", action="store_true", help="build static library", dest="static"
    )


def configure(cfg):
    # OSX/Mac uses .dylib and GNU/Linux .so
    cfg.env.SUFFIX = "dylib" if cfg.env["DEST_OS"] == "darwin" else "so"

    # Load C++ compiler configuration
    cfg.load("compiler_cxx clang_compilation_database")

    # Tools options
    cfg.options.magnum_components = (
        "Sdl2Application,Primitives,Shaders,MeshTools,SceneGraph,Trade,GL,DebugTools"
    )
    cfg.options.magnum_integrations = "Eigen,Bullet"

    # Load personal tools configurations
    for key in tools:
        cfg.load(key, tooldir=os.path.join(
            tools[key], "share/waf"))

    # Load external tools configurations
    cfg.load("flags magnum", tooldir="waf_tools")

    # Remove duplicates
    cfg.get_env()["libs"] = list(set(cfg.get_env()["libs"]))

    # Set lib type
    if cfg.options.shared:
        cfg.env["lib_type"] = "cxxshlib"
    else:
        cfg.env["lib_type"] = "cxxstlib"


def build(bld):
    # Library name
    bld.get_env()["libname"] = "Graphics"

    # Includes
    includes = []
    includes_path = "src"
    for root, _, filenames in os.walk(osp.join(bld.path.abspath(), includes_path)):
        for filename in filenames:
            if filename.endswith(('.hpp', '.h')):
                includes.append(os.path.join(root, filename))
    includes = [f[len(bld.path.abspath()) + 1:] for f in includes]

    # Sources
    sources = []
    sources_path = "src/graphics_lib"
    for root, dirnames, filenames in os.walk(
        osp.join(bld.path.abspath(), sources_path)
    ):
        for filename in fnmatch.filter(filenames, "*.cpp"):
            sources.append(os.path.join(root, filename))
    sources = " ".join([f[len(bld.path.abspath()) + 1:] for f in sources])

    # Build library
    if bld.options.shared:
        bld.shlib(
            features="cxx " + bld.env["lib_type"],
            source=sources,
            target=bld.get_env()["libname"],
            includes=includes_path,
            uselib=bld.get_env()["libs"],
            cxxxflags=bld.get_env()["CXXFLAGS"],
        )
    else:
        bld.stlib(
            features="cxx " + bld.env["lib_type"],
            source=sources,
            target=bld.get_env()["libname"],
            includes=includes_path,
            uselib=bld.get_env()["libs"],
            cxxxflags=bld.get_env()["CXXFLAGS"],
        )

    # Build examples
    bld.recurse("./src/examples")

    # Install headers
    for f in includes:
        end_index = f.rfind("/")
        if end_index == -1:
            end_index = len(f)
        bld.install_files("${PREFIX}/include/" + f[4:end_index], f)

    # Install libraries
    if bld.env["lib_type"] == "cxxstlib":
        bld.install_files(
            "${PREFIX}/lib", blddir + "/lib" + bld.get_env()["libname"] + ".a"
        )
    else:
        bld.install_files(
            "${PREFIX}/lib",
            blddir + "/lib" + bld.get_env()["libname"] + "." + bld.env.SUFFIX,
        )

    # Install tools
    bld.install_files("${PREFIX}/share/waf", "scripts/graphicslib.py")
    bld.install_files("${PREFIX}/share/waf", "waf_tools/utils.py")
