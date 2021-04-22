#include <iostream>

#include <magnum_dynamics/MagnumApp.hpp>
#include <magnum_dynamics/Object.hpp>

#include <Magnum/DebugTools/ColorMap.h>
#include <Magnum/DebugTools/ObjectRenderer.h>
#include <Magnum/DebugTools/ResourceManager.h>

using namespace magnum_dynamics;

int main(int argc, char** argv)
{
    MagnumApp app({argc, argv});

    app.addPrimitive("cube")
        .setPrimitiveTransformation(Matrix4())
        .setTransformation(Matrix4::translation({-2.0f, 0.0f, 0.0f}))
        .setColor(0xff0000_rgbf);

    app.addPrimitive("cube")
        .setPrimitiveTransformation(Matrix4::scaling({0.5f, 0.5f, 0.5f}))
        .setTransformation(Matrix4::translation({2.0f, 0.0f, 0.0f}))
        .setColor(0x00ff00_rgbf);

    return app.exec();
}