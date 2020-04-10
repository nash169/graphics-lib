#include <magnum_dynamics/MagnumApp.hpp>

using namespace Magnum;
using namespace Math::Literals;
using namespace magnum_dynamics;

int main(int argc, char** argv)
{
    MagnumApp app({argc, argv});

    // const Trade::MeshData3D cube = Primitives::cubeSolid();
    // app.addPrimitive(cube, Matrix4::translation(Vector3::xAxis(-3.0f)), 0xff0000_rgbf);
    // app.addPrimitive(cube, Matrix4::translation(Vector3::xAxis(3.0f)), 0x00ff00_rgbf);
    app.importScene("rsc/franka/link0.stl");
    app.importScene("rsc/franka/link1.stl");

    return app.exec();
}