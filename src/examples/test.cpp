#include <iostream>
#include <magnum_dynamics/MagnumApp.hpp>

using namespace magnum_dynamics;

int main(int argc, char** argv)
{
    MagnumApp app({argc, argv});

    Eigen::Vector3f a{1.0f, 2.0f, 3.0f};
    Vector3 b(a);

    const Trade::MeshData3D cube = Primitives::cubeSolid();
    app.addPrimitive(cube, Matrix4::translation(Vector3::xAxis(-3.0f)), 0xff0000_rgbf);
    app.addPrimitive(cube, Matrix4::translation(Vector3::xAxis(3.0f)), 0x00ff00_rgbf);
    // app.importScene("rsc/drone/s9_drone.stl");

    return app.exec();
}