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

#ifndef GRAPHICSLIB_TOOLS_HELPER_HPP
#define GRAPHICSLIB_TOOLS_HELPER_HPP

namespace graphics_lib {
    namespace tools {
        // Get colors
        template <typename ColorType = Color4>
        inline ColorType color(const std::string& color)
        {
            if (!color.compare("red"))
                return ColorType::red();
            else if (!color.compare("green"))
                return ColorType::green();
            else if (!color.compare("blue"))
                return ColorType::blue();
            else if (!color.compare("cyan"))
                return ColorType::cyan();
            else if (!color.compare("magenta"))
                return ColorType::magenta();
            else if (!color.compare("yellow"))
                return ColorType::yellow();
            else if (!color.compare("white"))
                return ColorType(0xffffff_rgbf);
            else if (!color.compare("grey"))
                return ColorType(0x585858_rgbf);
            else if (!color.compare("black"))
                return ColorType();
            else {
                std::cerr << "Color not found; setting default color WHITE." << std::endl;
                return ColorType(0xffffff_rgbf);
            }
        }
    } // namespace tools
} // namespace graphics_lib

#endif // GRAPHICSLIB_TOOLS_HELPER_HPP