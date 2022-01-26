#! /usr/bin/env python
# encoding: utf-8

from waflib.Configure import conf
from utils import check_include, check_lib


def options(opt):
    # Required package options
    opt.load("eigen magnum", tooldir="waf_tools")

    # Options
    opt.add_option(
        "--science_graphics-path",
        type="string",
        help="path to science-graphics",
        dest="science_graphics_path",
    )


@conf
def check_science_graphics(ctx):
    # Set the search path
    if ctx.options.science_graphics_path is None:
        path_check = ["/usr/local", "/usr"]
    else:
        path_check = [ctx.options.science_graphics_path]

    # science-graphics includes
    check_include(
        ctx, "SCIENCEGRAPHICS", [""], [
            "science_graphics/ScienceGraphics.hpp"], path_check
    )

    # science-graphics libs
    check_lib(ctx, "SCIENCEGRAPHICS", "", ["libScienceGraphics"], path_check)

    if ctx.env.LIB_SCIENCEGRAPHICS or ctx.env.STLIB_SCIENCEGRAPHICS:
        # Add dependencies to require libraries
        ctx.get_env()["requires"] = ctx.get_env()[
            "requires"] + ["EIGEN", "MAGNUM"]

        # Check for dependencies
        ctx.options.magnum_components = (
            "Sdl2Application,Primitives,Shaders,MeshTools,SceneGraph,Trade,GL,DebugTools"
        )
        ctx.options.magnum_integrations = "Eigen,Bullet"

        ctx.load("eigen magnum", tooldir="waf_tools")

        # Add useful define to dynamically activate the graphics
        ctx.env.DEFINES_SCIENCEGRAPHICS += ["GRAPHICS"]

        # Add library
        ctx.get_env()["libs"] = ctx.get_env()["libs"] + ["SCIENCEGRAPHICS"]


def configure(cfg):
    if not cfg.env.LIB_SCIENCEGRAPHICS and not cfg.env.STLIB_SCIENCEGRAPHICS:
        cfg.check_science_graphics()
