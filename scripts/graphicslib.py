#!/usr/bin/env python
# encoding: utf-8
#
#    This file is part of graphics-lib.
#
#    Copyright (c) 2020, 2021, 2022 Bernardo Fichera <bernardo.fichera@gmail.com>
#
#    Permission is hereby granted, free of charge, to any person obtaining a copy
#    of this software and associated documentation files (the "Software"), to deal
#    in the Software without restriction, including without limitation the rights
#    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#    copies of the Software, and to permit persons to whom the Software is
#    furnished to do so, subject to the following conditions:
#
#    The above copyright notice and this permission notice shall be included in all
#    copies or substantial portions of the Software.
#
#    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#    SOFTWARE.

from waflib.Configure import conf
from utils import check_include, check_lib


def options(opt):
    # Required package options
    opt.load("eigen magnum", tooldir="waf_tools")

    # Options
    opt.add_option(
        "--graphicslib-path",
        type="string",
        help="path to graphics-lib",
        dest="graphicslib_path",
    )


@conf
def check_graphicslib(ctx):
    # Set the search path
    if ctx.options.graphicslib_path is None:
        path_check = ["/usr/local", "/usr"]
    else:
        path_check = [ctx.options.graphicslib_path]

    # graphics-lib includes
    check_include(
        ctx, "GRAPHICSLIB", [""], [
            "graphics_lib/Graphics.hpp"], path_check
    )

    # graphics-lib libs
    check_lib(ctx, "GRAPHICSLIB", "", ["libGraphics"], path_check)

    if ctx.env.LIB_GRAPHICSLIB or ctx.env.STLIB_GRAPHICSLIB:
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
        ctx.env.DEFINES_GRAPHICSLIB += ["GRAPHICS"]

        # Add library
        ctx.get_env()["libs"] = ctx.get_env()["libs"] + ["GRAPHICSLIB"]


def configure(cfg):
    if not cfg.env.LIB_GRAPHICSLIB and not cfg.env.STLIB_GRAPHICSLIB:
        cfg.check_graphicslib()
