#include <iostream>

#include <magnum_dynamics/MagnumApp.hpp>

using namespace magnum_dynamics;

int main(int argc, char** argv)
{
    MagnumApp app({argc, argv});

    // Eigen::Vector3f a{1.0f, 2.0f, 3.0f};
    // Vector3 b(a);

    app.add("cube", "", Matrix4::scaling({0.5f, 0.5f, 0.5f}) * Matrix4::translation({0.0f, -2.0f, 0.0f}), 0xff0000_rgbf);
    app.add("cube", "", Matrix4::scaling({0.5f, 0.5f, 0.5f}) * Matrix4::translation({0.0f, 2.0f, 0.0f}), 0x00ff00_rgbf);
    // app.add("rsc/franka/link0.dae");

    // std::cout << app.getObjects().size() << std::endl;

    return app.exec();
}