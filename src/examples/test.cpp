#include <iostream>

#include <magnum_dynamics/MagnumApp.hpp>

// #include <Magnum/DebugTools/ObjectRenderer.h>
// #include <Magnum/DebugTools/ResourceManager.h>

using namespace magnum_dynamics;

int main(int argc, char** argv)
{
    MagnumApp app({argc, argv});

    // app.add("cube", "", Matrix4::scaling({0.5f, 0.5f, 0.5f}) * Matrix4::translation({0.0f, -2.0f, 0.0f}), 0xff0000_rgbf);
    // app.add("cube", "", Matrix4::scaling({0.5f, 0.5f, 0.5f}) * Matrix4::translation({0.0f, 2.0f, 0.0f}), 0x00ff00_rgbf);

    app.add("rsc/franka/link1.dae");

    // DebugTools::ResourceManager manager;
    // manager.set("my", DebugTools::ObjectRendererOptions{}.setSize(0.3f));

    // auto debug = new DebugTools::ObjectRenderer3D{manager, *app.manipulator().children().last(), my, &app.drawables()};

    return app.exec();
}