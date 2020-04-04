#include "magnum_app.hpp"

using namespace Magnum;
using namespace Math::Literals;

int main(int argc, char** argv)
{
    MagnumApp app({argc, argv});

    // const Trade::MeshData3D cube = Primitives::cubeSolid();
    // app.addPrimitive(cube, Matrix4::translation(Vector3::xAxis(-3.0f)), 0xff0000_rgbf);
    // app.addPrimitive(cube, Matrix4::translation(Vector3::xAxis(3.0f)), 0x00ff00_rgbf);

    return app.exec();  
}