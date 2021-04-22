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

    app.import("rsc/franka/link0.dae");
    // app.import("rsc/iiwa/link_0.obj");

    app.addFrame()
        .setTransformation(Matrix4());

    // DebugTools::ResourceManager manager;
    // manager.set("my", DebugTools::ObjectRendererOptions{}.setSize(0.3f));

    // auto debug = new DebugTools::ObjectRenderer3D{manager, *app.manipulator().children().last(), "my", &app.drawables()};

    return app.exec();
}