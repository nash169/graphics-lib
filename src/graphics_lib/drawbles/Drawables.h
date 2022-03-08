/*
    This file is part of graphics-lib.

    Copyright (c) 2020, 2021, 2022 Bernardo Fichera <bernardo.fichera@gmail.com>

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#ifndef GRAPHICSLIB_DRAWABLES_DRAWABLES_H
#define GRAPHICSLIB_DRAWABLES_DRAWABLES_H

namespace graphics_lib {
    namespace drawables {
        template <size_t>
        class AbstractDrawable;
        typedef AbstractDrawable<3> AbstractDrawable3D;
        typedef AbstractDrawable<2> AbstractDrawable2D;

        template <size_t>
        class ColorDrawable;
        typedef ColorDrawable<3> ColorDrawable3D;
        typedef ColorDrawable<2> ColorDrawable2D;

        template <size_t>
        class PhongDrawable;
        typedef PhongDrawable<3> PhongDrawable3D;
        typedef PhongDrawable<2> PhongDrawable2D;

        template <size_t>
        class TextureDrawable;
        typedef TextureDrawable<3> TextureDrawable3D;
        typedef TextureDrawable<2> TextureDrawable2D;
    } // namespace drawables
} // namespace graphics_lib

#endif // GRAPHICSLIB_DRAWABLES_DRAWABLES_H