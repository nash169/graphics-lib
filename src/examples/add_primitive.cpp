#include <iostream>

#include <magnum_dynamics/MagnumApp.hpp>
// #include <magnum_dynamics/Object.hpp>

// #include <Magnum/DebugTools/ColorMap.h>
// #include <Magnum/DebugTools/ObjectRenderer.h>
// #include <Magnum/DebugTools/ResourceManager.h>
#include <magnum_dynamics/tools/helper.hpp>

using namespace magnum_dynamics;

int main(int argc, char** argv)
{
    MagnumApp app({argc, argv});

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

    app.camera()->setPose({5., 0., 5.});

    return app.exec();
}