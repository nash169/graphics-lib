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

#include <graphics_lib/Graphics.hpp>

using namespace graphics_lib;

int main(int argc, char** argv)
{
    Graphics app({argc, argv});

    // app.addFrame();

    app.addPrimitive("cube")
        .addPriorTransformation(Matrix4::scaling({0.5f, 0.5f, 0.5f}))
        .setColor(Color4::red()) // 0xff0000_rgbf
        .setTransformation(Matrix4::translation({0.0f, -2.0f, 0.0f}));

    app.addPrimitive("capsule")
        .addPriorTransformation(Matrix4::scaling({0.5f, 0.5f, 0.5f}))
        .setColor(Color4::green())
        .setTransformation(Matrix4::translation({0.0f, 0.0f, 0.0f}));

    app.addPrimitive("cone")
        .addPriorTransformation(Matrix4::scaling({0.5f, 0.5f, 0.5f}))
        .setColor(Color4::blue())
        .setTransformation(Matrix4::translation({0.0f, 2.0f, 0.0f}));

    app.addPrimitive("cylinder")
        .addPriorTransformation(Matrix4::scaling({0.5f, 0.5f, 0.5f}))
        .setColor(Color4::yellow())
        .setTransformation(Matrix4::translation({0.0f, -2.0f, 2.0f}));

    app.addPrimitive("sphere")
        .addPriorTransformation(Matrix4::scaling({0.5f, 0.5f, 0.5f}))
        .setColor(Color4::cyan())
        .setTransformation(Matrix4::translation({0.0f, 2.0f, 2.0f}));

    app.camera3D().setPose(Vector3{5., 0., 5.});

    return app.exec();
}